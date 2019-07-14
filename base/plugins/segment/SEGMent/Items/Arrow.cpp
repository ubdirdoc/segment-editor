#include "Arrow.hpp"

#include <score/selection/SelectionStack.hpp>

#include <QGraphicsSceneMouseEvent>
#include <QPen>
#include <QtMath>

#include <SEGMent/Commands/Properties.hpp>
namespace SEGMent
{

Arrow::Arrow(
    const TransitionModel& t,
    Anchor& startItem,
    Anchor& endItem,
    const score::DocumentContext& c,
    ZoomView& view,
    QGraphicsItem* parentItem)
    : QGraphicsLineItem(parentItem)
    , context{c}
    , m_model{t}
    , m_startItem{startItem}
    , m_endItem{endItem}
{
    setVisible(false);
  setAcceptDrops(true);
  con(t.selection, &Selectable::changed, this, &Arrow::setSelected);
  connect(&startItem, &Anchor::moved, this, &Arrow::updateShape);
  connect(&endItem, &Anchor::moved, this, &Arrow::updateShape);
  updateShape();
}

QRectF Arrow::boundingRect() const
{
  qreal extra = (pen().width() + 20) / 2.0;

  return QRectF(
             line().p1(),
             QSizeF(
                 line().p2().x() - line().p1().x(),
                 line().p2().y() - line().p1().y()))
      .normalized()
      .adjusted(-extra, -extra, extra, extra);
}

QPainterPath Arrow::shape() const
{
  QPainterPath path;
  path.moveTo(line().p1());
  path.lineTo(line().p2());

  QPainterPathStroker stk;
  stk.setWidth(50);
  return stk.createStroke(path);
}

void Arrow::paint(
    QPainter* painter,
    const QStyleOptionGraphicsItem* option,
    QWidget* widget)
{
  QLineF l = line();
  const auto& style = m_model.transition().target<SceneToScene>()
                          ? Style::instance().sceneArrow
                          : Style::instance().arrow;

  if (m_dragging)
  {
    painter->setPen(style.hoverPen);
    painter->setBrush(style.hoverBrush);

    painter->drawLine(l);
    painter->drawPolygon(m_arrowHead);
  }
  else if (!m_selected)
  {
    painter->setPen(style.pen);
    painter->setBrush(style.brush);

    painter->drawLine(l);
    painter->drawPolygon(m_arrowHead);
  }
  else
  {
    painter->setPen(style.selectedPen);
    painter->setBrush(style.selectedBrush);

    painter->drawLine(l);
    painter->drawPolygon(m_arrowHead);
  }
}

void Arrow::updateShape()
{
  QPointF startPoint = m_startItem.scenePos();
  QPointF endPoint = m_endItem.scenePos();

  startPoint.setX(startPoint.x() + m_startItem.boundingRect().width() / 2.);
  startPoint.setY(startPoint.y() + m_startItem.boundingRect().height() / 2.);

  endPoint.setX(endPoint.x() + m_endItem.boundingRect().width() / 2.);
  endPoint.setY(endPoint.y() + m_endItem.boundingRect().height() / 2.);

  setLine(QLineF(endPoint, startPoint));

  using namespace std;
  constexpr qreal arrowSize = 20;
  const double angle = std::atan2(-line().dy(), line().dx());
  QPointF arrowP1 = endPoint
                    + QPointF(
                        sin(angle + M_PI / 3.) * arrowSize,
                        cos(angle + M_PI / 3.) * arrowSize);
  QPointF arrowP2 = endPoint
                    + QPointF(
                        sin(angle + M_PI - M_PI / 3.) * arrowSize,
                        cos(angle + M_PI - M_PI / 3.) * arrowSize);

  m_arrowHead.clear();
  m_arrowHead << line().p1() << arrowP1 << arrowP2;
}

void Arrow::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
  if (shape().contains(event->pos()))
  {
    context.selectionStack.pushNewSelection({&m_model});
    event->setAccepted(true);
  }
  else
  {
    event->setAccepted(false);
  }
}

void Arrow::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
  event->setAccepted(false);
}

void Arrow::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
  event->setAccepted(false);
  if ((event->buttons() & Qt::RightButton)
      || (event->button() == Qt::RightButton))
  {
    auto m = new QMenu;
    auto act = m->addAction(tr("Delete"));
    connect(act, &QAction::triggered, [=] {
      CommandDispatcher<> c{this->context.commandStack};
      c.submitCommand(new RemoveTransition{
          (const SEGMent::ProcessModel&)*m_model.parent(), m_model});
    });
    m->exec(event->screenPos());
    m->deleteLater();
  }
}

void Arrow::dragEnterEvent(QGraphicsSceneDragDropEvent* event)
{
  const auto& txt = event->mimeData()->text();

  bool can_drop = txt == SEGMENT_SOUND_ID;
  if (!can_drop)
  {
    if (m_model.transition().target<SEGMent::SceneToScene>())
    {
      can_drop = txt == SEGMENT_TEXTRIDDLE_ID || txt == SEGMENT_GIFRIDDLE_ID
                 || txt == SEGMENT_PUZZLERIDDLE_ID;
    }
  }

  if (can_drop)
  {
    m_dragging = true;
    event->accept();
    update();
  }
  else
  {
    event->ignore();
  }
}

void Arrow::dragMoveEvent(QGraphicsSceneDragDropEvent* event)
{
  event->accept();
}

void Arrow::dragLeaveEvent(QGraphicsSceneDragDropEvent* event)
{
  m_dragging = false;
  event->accept();
  update();
}

void Arrow::dropEvent(QGraphicsSceneDragDropEvent* event)
{
  m_dragging = false;
  event->accept();
  update();

  auto mime = event->mimeData();
  const auto& txt = mime->text();
  if (txt == SEGMENT_SOUND_ID)
  {
    if (!mime->urls().empty())
    {
      QUrl currentUrl = mime->urls().first();

      CommandDispatcher<> disp{context.commandStack};
      disp.submitCommand(new SetTransitionSound{m_model, currentUrl.path()});
    }
  }
  else
  {
    auto trans = m_model.transition().target<SEGMent::SceneToScene>();
    if (trans)
    {
      if (txt == SEGMENT_TEXTRIDDLE_ID)
      {
        CommandDispatcher<> disp{context.commandStack};
        if (!trans->riddle.target<SEGMent::TextRiddle>())
        {
          auto r = trans->riddle;
          static_cast<riddle_t::impl_type&>(r) = SEGMent::TextRiddle{};
          disp.submitCommand(new ChangeRiddle{m_model, std::move(r)});
        }
      }
      else if (txt == SEGMENT_GIFRIDDLE_ID)
      {
        CommandDispatcher<> disp{context.commandStack};
        if (!trans->riddle.target<SEGMent::GifRiddle>())
        {
          auto r = trans->riddle;
          static_cast<riddle_t::impl_type&>(r) = SEGMent::GifRiddle{};
          disp.submitCommand(new ChangeRiddle{m_model, std::move(r)});
        }
      }
      else if (txt == SEGMENT_PUZZLERIDDLE_ID)
      {
        CommandDispatcher<> disp{context.commandStack};
        if (!trans->riddle.target<SEGMent::PuzzleRiddle>())
        {
          auto r = trans->riddle;
          static_cast<riddle_t::impl_type&>(r) = SEGMent::PuzzleRiddle{};
          disp.submitCommand(new ChangeRiddle{m_model, std::move(r)});
        }
      }
    }
  }
}

void Arrow::setSelected(bool b)
{
  m_selected = b;
  update();
}

} // namespace SEGMent
