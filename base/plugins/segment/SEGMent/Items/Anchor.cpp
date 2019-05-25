#include "Anchor.hpp"

#include "Arrow.hpp"

#include <QDrag>
#include <QCursor>
#include <QGraphicsSceneMouseEvent>
#include <QMimeData>
#include <QApplication>
#include <SEGMent/Model/Layer/ProcessView.hpp>
#include <wobjectimpl.h>

W_OBJECT_IMPL(SEGMent::Anchor)
namespace SEGMent
{

Anchor::Anchor(qreal w, anchor_id i, QGraphicsItem* parent)
    : QGraphicsItem(parent)
    , id{i}
    , m_processView{*GetParentSEGMentView(this)}
    , m_width{w}
{
  setFlag(ItemSendsScenePositionChanges);
  setFlag(ItemIsSelectable);
  setFlag(ItemIgnoresTransformations);
  setCursor(Qt::CrossCursor);
  setZValue(10);
  setAcceptDrops(true);
}

QRectF Anchor::boundingRect() const
{
  const auto w = m_width + 2.;
  return { -w / 2., -w / 2., w, w };
}

void Anchor::dragEnterEvent(QGraphicsSceneDragDropEvent* event)
{
  // TODO cursor
  QGraphicsItem::dragEnterEvent(event);
  prepareGeometryChange();
  m_width *= 1.5;
  update();
}

void Anchor::dropEvent(QGraphicsSceneDragDropEvent* event)
{
  m_processView.setEndAnchorForNewArrow(*this);
  m_processView.dropEvent(event);
  prepareGeometryChange();
  m_width /= 1.5;
  update();
}

void Anchor::dragLeaveEvent(QGraphicsSceneDragDropEvent* event)
{
  QGraphicsItem::dragLeaveEvent(event);
  prepareGeometryChange();
  m_width /= 1.5;
  update();
}

void Anchor::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
  event->accept();
}

void Anchor::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
  event->accept();
}

void Anchor::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
  m_processView.setStartAnchorForNewArrow(*this);

  auto drag = new QDrag{event->widget()};
  auto mime = new QMimeData;

  drag->setMimeData(mime);
  connect(drag, &QDrag::destroyed, this, [=] {
    m_processView.finishArrowDrop();
  });
  drag->exec();
}

QVariant Anchor::itemChange(
      QGraphicsItem::GraphicsItemChange change,
      const QVariant& value)
{
  if(change == GraphicsItemChange::ItemScenePositionHasChanged)
    moved();
  return QGraphicsItem::itemChange(change, value);
}

void Anchor::paint(
      QPainter* painter,
      const QStyleOptionGraphicsItem* option,
      QWidget* widget)
{
  //painter->setRenderHint(QPainter::Antialiasing);
  //QGraphicsEllipseItem::paint(painter, option, widget);
  const auto& skin = Style::instance();
  painter->setPen(skin.anchorPen);
  painter->setBrush(skin.anchorBrush);
  const auto w = m_width;
  painter->drawEllipse( QRectF{ -w / 2., -w / 2., w, w });
}

QPainterPath Anchor::shape() const
{
  QPainterPath p;
  p.addRect(boundingRect());
  return p;
}

bool Anchor::contains(const QPointF& point) const
{
  return boundingRect().contains(point);
}
}

