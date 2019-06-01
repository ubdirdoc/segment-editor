#include "ProcessView.hpp"

#include <score/command/Dispatchers/CommandDispatcher.hpp>
#include <score/document/DocumentContext.hpp>

#include <core/document/DocumentModel.hpp>

#include <QApplication>
#include <QDir>
#include <QGraphicsPixmapItem>
#include <QGraphicsProxyWidget>
#include <QGraphicsRectItem>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsView>
#include <QHBoxLayout>
#include <QJsonDocument>
#include <QMdiArea>
#include <QMimeData>
#include <QPainter>
#include <QPixmap>
#include <QPushButton>
#include <QRectF>
#include <QTabWidget>
#include <QtMath>

#include <SEGMent/Commands/CommandFactory.hpp>
#include <SEGMent/Commands/Creation.hpp>
#include <SEGMent/Document.hpp>
#include <SEGMent/Items/Arrow.hpp>
#include <SEGMent/Items/ObjectWindow.hpp>
#include <wobjectimpl.h>

// W_OBJECT_IMPL(SEGMent::View)
W_OBJECT_IMPL(SEGMent::ZoomView)

namespace SEGMent
{

ZoomView::ZoomView(const score::DocumentContext& ctx)
    : context{ctx}
    , m_process{safe_cast<DocumentModel&>(ctx.document.model().modelDelegate())
                    .process()}
{
  setDragMode(QGraphicsView::DragMode::ScrollHandDrag);
}

void ZoomView::enterEvent(QEvent* event)
{
  QGraphicsView::enterEvent(event);
  viewport()->unsetCursor();
}

void ZoomView::mousePressEvent(QMouseEvent* e)
{
  QGraphicsView::mousePressEvent(e);
  auto p = this->mapToScene(e->pos());
  if (QGraphicsItem* item = this->scene()->itemAt(p, transform()); !item)
  {
    viewport()->setCursor(Qt::ClosedHandCursor);
    context.selectionStack.deselect();
  }
}

void ZoomView::mouseMoveEvent(QMouseEvent* e)
{
  QGraphicsView::mouseMoveEvent(e);

  if (e->buttons() & Qt::LeftButton)
  {
    auto top_left = mapToScene(QPoint{0, 0});
    auto bottom_right = mapToScene(QPoint{width(), height()});
    scene()->update(QRectF{top_left, bottom_right});
  }
}

void ZoomView::mouseReleaseEvent(QMouseEvent* event)
{
  QGraphicsView::mouseReleaseEvent(event);
  viewport()->unsetCursor();
  update();
}

void ZoomView::wheelEvent(QWheelEvent* event)
{
  if (event->modifiers() & Qt::ControlModifier)
  {
    zoom(event->angleDelta().y());
  }
  else
  {
    QGraphicsView::wheelEvent(event);
  }
}

void ZoomView::drawBackground(QPainter* painter, const QRectF& s)
{
  QBrush b = SEGMent::Style::instance().backgroundPen;

  b.setTransform(QTransform(painter->worldTransform().inverted()).scale(2, 2));
  painter->setBackground(SEGMent::Style::instance().backgroundBrush);
  painter->setBackgroundMode(Qt::OpaqueMode);
  painter->setBrush(b);
  painter->fillRect(s, b);
}

tsl::hopscotch_map<QGraphicsItem*, bool> visibility_map;
void ZoomView::setStartAnchorForNewArrow(Anchor& startAnchor)
{
  m_startAnchorForNewArrow = &startAnchor;
  auto anchors = this->scene()->items();
  for (auto scene : m_sceneWindows)
  {
    for (auto anchor : scene->anchors())
    {
      visibility_map[anchor] = anchor->isVisible();
      anchor->setVisible(true);
    }
  }
}

void ZoomView::setEndAnchorForNewArrow(Anchor& endAnchor)
{
  if (m_startAnchorForNewArrow == nullptr)
  {
    return;
  }

  if (m_startAnchorForNewArrow->parentItem() != endAnchor.parentItem())
  {
    auto source
        = dynamic_cast<Window*>(m_startAnchorForNewArrow->parentItem());
    if (!source)
      return;
    auto target = endAnchor.parentItem();
    if (target->type() != SceneWindow::static_type())
      return;

    auto& target_model = static_cast<SceneWindow*>(target)->model();

    transition_t trans = source->createTransitionFrom(
        m_startAnchorForNewArrow->id, target_model, endAnchor.id);

    CommandDispatcher<> c{context.commandStack};
    c.submitCommand(new CreateTransition(m_process, trans));
  }
}

void ZoomView::finishArrowDrop()
{
  delete m_tmpArrow;
  m_tmpArrow = nullptr;
  m_startAnchorForNewArrow = nullptr;

  for (auto scene : m_sceneWindows)
  {
    for (auto anchor : scene->anchors())
    {
      anchor->setVisible(visibility_map[anchor]);
    }
  }

  visibility_map.clear();
}

void ZoomView::addScene(SceneWindow* s)
{
  scene()->addItem(s);
  m_sceneWindows.push_back(s);
  update();
}

void ZoomView::removeScene(const SceneWindow* s)
{
  auto it = ossia::find(m_sceneWindows, s);
  if (it != m_sceneWindows.end())
  {
    m_sceneWindows.erase(it);
  }

  delete s;
}

void ZoomView::addTransition(Arrow* a)
{
  scene()->addItem(a);
  m_sceneArrows.push_back(a);
  update();
}

void ZoomView::removeTransition(const Arrow* a)
{
  auto it = ossia::find(m_sceneArrows, a);
  if (it != m_sceneArrows.end())
  {
    m_sceneArrows.erase(it);
  }

  delete a;
}

struct FakeArrow final : QGraphicsLineItem
{
  FakeArrow(qreal x1, qreal y1, qreal x2, qreal y2)
      : QGraphicsLineItem{x1, y1, x2, y2}
  {
  }

  QRectF boundingRect() const override
  {
    auto r = QGraphicsLineItem::boundingRect();
    return r.adjusted(-10, -10, 10, 10);
  }

  void paint(
      QPainter* painter,
      const QStyleOptionGraphicsItem* option,
      QWidget* widget) override
  {
    qreal arrowSize = 20;
    painter->setPen(Style::instance().arrow.draggedPen);
    painter->setBrush(Style::instance().arrow.draggedBrush);
    // painter->setRenderHints(QPainter::Antialiasing);
    painter->drawLine(line());

    double angle = std::atan2(-line().dy(), line().dx());

    QPointF arrowP1 = line().p1()
                      + QPointF(
                          sin(angle + M_PI / 3) * arrowSize,
                          cos(angle + M_PI / 3) * arrowSize);
    QPointF arrowP2 = line().p1()
                      + QPointF(
                          sin(angle + M_PI - M_PI / 3) * arrowSize,
                          cos(angle + M_PI - M_PI / 3) * arrowSize);

    QPolygonF m_arrowHead;
    m_arrowHead << line().p1() << arrowP1 << arrowP2;
    painter->drawLine(line());
    painter->drawPolygon(m_arrowHead);
  }
};

void ZoomView::dragMove(QPointF scenePos)
{
  if (m_startAnchorForNewArrow != nullptr)
  {
    auto start = scenePos;
    auto end = m_startAnchorForNewArrow->mapToScene(
        m_startAnchorForNewArrow->boundingRect().center());

    if (m_tmpArrow == nullptr)
    {
      m_tmpArrow = new FakeArrow{start.x(), start.y(), end.x(), end.y()};

      scene()->addItem(m_tmpArrow);
      m_tmpArrow->setZValue(10);
    }

    m_tmpArrow->setLine(start.x(), start.y(), end.x(), end.y());
  }
}

void ZoomView::dragMoveEvent(QDragMoveEvent* event)
{
  dragMove(this->mapToScene(event->pos()));
  QGraphicsView::dragMoveEvent(event);
}

void ZoomView::dropEvent(QDropEvent* event)
{
  const QPointF localPos = mapToScene(event->pos());
  const QMimeData* currentMimeData = event->mimeData();

  if (currentMimeData->hasText() && currentMimeData->hasUrls())
  {
    if (currentMimeData->text() == SEGMENT_SCENE_ID)
    {
      const QRectF rect{localPos.x() - 200, localPos.y() - 200, 400, 400};
      CommandDispatcher<> disp{context.commandStack};

      QUrl currentUrl = currentMimeData->urls().first();
      disp.submitCommand(
          new DropScene{m_process, Image{currentUrl.path()}, rect});
    }
    else if (currentMimeData->text() == SEGMENT_TEMPLATE_ID)
    {
      QUrl currentUrl = currentMimeData->urls().first();
      auto file = toLocalFile(currentUrl.path(), context);
      if (QFile f{file}; f.open(QIODevice::ReadOnly))
      {
        auto json = QJsonDocument::fromJson(f.readAll()).object();
        json.remove("Pixmap");
        auto rect = fromJsonValue<QRectF>(json["Rect"]);
        rect = {localPos.x() - rect.width() / 2.,
                localPos.y() - rect.width() / 2.,
                rect.width(),
                rect.height()};
        json["Rect"] = toJsonValue(rect);

        CommandDispatcher<> disp{context.commandStack};

        disp.submitCommand(new PasteScene{m_process, json});
      }
    }
  }

  QGraphicsView::dropEvent(event);
}

} // namespace SEGMent
