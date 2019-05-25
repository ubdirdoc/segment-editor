#include "ProcessView.hpp"

#include <QApplication>
#include <QDir>
#include <QGraphicsPixmapItem>
#include <QGraphicsProxyWidget>
#include <QGraphicsRectItem>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsView>
#include <QHBoxLayout>
#include <QMdiArea>
#include <QMimeData>
#include <QPainter>
#include <QPixmap>
#include <QPushButton>
#include <QRectF>
#include <QTabWidget>
#include <QtMath>
#include <SEGMent/Commands/CommandFactory.hpp>
#include <SEGMent/Items/Arrow.hpp>
#include <SEGMent/Items/ObjectWindow.hpp>
#include <score/command/Dispatchers/CommandDispatcher.hpp>
#include <SEGMent/Commands/Creation.hpp>
#include <QJsonDocument>
#include <wobjectimpl.h>
#include <score/document/DocumentContext.hpp>

W_OBJECT_IMPL(SEGMent::View)

namespace SEGMent
{
View* GetParentSEGMentView(QGraphicsItem* object)
{
  while (object)
  {
    if(object->type() == View::static_type())
      return static_cast<View*>(object);

    object = object->parentItem();
  }

  return nullptr;
}

View::View(const ProcessModel& proc, const score::DocumentContext& ctx, QGraphicsItem* parent)
  : QGraphicsItem{parent}
  , m_process{proc}
  , m_ctx{ctx}
{
  QGraphicsItem::setAcceptHoverEvents(false);

  this->setFlag(QGraphicsItem::ItemClipsToShape, false);
  this->setFlag(QGraphicsItem::ItemClipsChildrenToShape, false);
  this->setFlag(QGraphicsItem::ItemContainsChildrenInShape, false);
  this->setFlag(QGraphicsItem::ItemIsSelectable, false);
  this->setFlag(QGraphicsItem::ItemHasNoContents, true);
  this->setFlag(QGraphicsItem::ItemIgnoresTransformations, false);

  this->setAcceptDrops(true);
}

void View::dropEvent(QGraphicsSceneDragDropEvent* event)
{
  const QMimeData* currentMimeData = event->mimeData();

  if (currentMimeData->hasText() && currentMimeData->hasUrls())
  {
    if (currentMimeData->text() == SEGMENT_SCENE_ID)
    {
      const QPointF localPos = event->pos();
      const QRectF rect{localPos.x(), localPos.y(), 400, 400};
      CommandDispatcher<> disp{m_ctx.commandStack};

      QUrl currentUrl = currentMimeData->urls().first();
      disp.submitCommand(new DropScene{m_process, Image{currentUrl.path()}, rect});
    }
    else if (currentMimeData->text() == SEGMENT_TEMPLATE_ID)
    {
      QUrl currentUrl = currentMimeData->urls().first();
      const QPointF localPos = event->pos();
      auto file = toLocalFile(currentUrl.path(), m_ctx);
      if(QFile f{file}; f.open(QIODevice::ReadOnly))
      {
        auto json = QJsonDocument::fromJson(f.readAll()).object();
        json.remove("Pixmap");
        auto rect = fromJsonValue<QRectF>(json["Rect"]);
        rect = {localPos.x(), localPos.y(), rect.width(), rect.height()};
        json["Rect"] = toJsonValue(rect);

        CommandDispatcher<> disp{m_ctx.commandStack};

        disp.submitCommand(new PasteScene{m_process, json});
      }
    }
  }
}

tsl::hopscotch_map<QGraphicsItem*, bool> visibility_map;
void View::setStartAnchorForNewArrow(Anchor& startAnchor)
{
  m_startAnchorForNewArrow = &startAnchor;
  auto anchors = this->scene()->items();
  for(auto scene : m_sceneWindows)
  {
    for(auto anchor : scene->anchors())
    {
      visibility_map[anchor] = anchor->isVisible();
      anchor->setVisible(true);
    }
  }
}

void View::setEndAnchorForNewArrow(Anchor& endAnchor)
{
  if (m_startAnchorForNewArrow == nullptr)
  {
    return;
  }

  if (m_startAnchorForNewArrow->parentItem() != endAnchor.parentItem())
  {
    auto source = dynamic_cast<Window*>(m_startAnchorForNewArrow->parentItem());
    if(!source)
      return;
    auto target = endAnchor.parentItem();
    if(target->type() != SceneWindow::static_type())
      return;

    auto& target_model = static_cast<SceneWindow*>(target)->model();

    transition_t trans = source->createTransitionFrom(m_startAnchorForNewArrow->id, target_model, endAnchor.id);

    CommandDispatcher<> c{m_ctx.commandStack};
    c.submitCommand(new CreateTransition(m_process, trans));
  }

}

void View::finishArrowDrop()
{
  delete m_tmpArrow;
  m_tmpArrow = nullptr;
  m_startAnchorForNewArrow = nullptr;

  for(auto scene : m_sceneWindows)
  {
    for(auto anchor : scene->anchors())
    {
      anchor->setVisible(visibility_map[anchor]);
    }
  }

  visibility_map.clear();
}


void View::addScene(SceneWindow* s)
{
  m_sceneWindows.push_back(s);
  update();
}

void View::removeScene(const SceneWindow* s)
{
  auto it = ossia::find(m_sceneWindows, s);
  if(it != m_sceneWindows.end())
  {
    m_sceneWindows.erase(it);
  }

  delete s;
}

void View::addTransition(Arrow* a)
{
  m_sceneArrows.push_back(a);
  update();
}

void View::removeTransition(const Arrow* a)
{
  auto it = ossia::find(m_sceneArrows, a);
  if(it != m_sceneArrows.end())
  {
    m_sceneArrows.erase(it);
  }

  delete a;
}

void View::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
}

struct FakeArrow final : QGraphicsLineItem
{
  FakeArrow(qreal x1, qreal y1, qreal x2, qreal y2, QGraphicsItem* parent)
    : QGraphicsLineItem{x1, y1, x2, y2, parent}
  {

  }

  QRectF boundingRect() const override
  {
    auto r = QGraphicsLineItem::boundingRect();
    return r.adjusted(-10, -10, 10, 10);
  }

  void paint(
      QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override
  {
    qreal arrowSize = 20;
    painter->setPen(Style::instance().arrow.draggedPen);
    painter->setBrush(Style::instance().arrow.draggedBrush);
    //painter->setRenderHints(QPainter::Antialiasing);
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

void View::dragMoveEvent(QGraphicsSceneDragDropEvent* event)
{
  if (m_startAnchorForNewArrow != nullptr)
  {
    auto start = this->mapFromScene(event->scenePos());
    auto end = m_startAnchorForNewArrow->mapToItem(this, m_startAnchorForNewArrow->boundingRect().center());

    if (m_tmpArrow == nullptr)
    {
      m_tmpArrow
          = new FakeArrow{
            start.x(), start.y(),
            end.x(), end.y(),
            this};

      m_tmpArrow->setZValue(10);
    }

    m_tmpArrow->setLine(start.x(), start.y(), end.x(), end.y());
  }
}

}
