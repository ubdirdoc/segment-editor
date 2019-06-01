#include <SEGMent/Items/RectItem.hpp>

#include <SEGMent/Model/Layer/ProcessView.hpp>
#include <SEGMent/Items/Window.hpp>

#include <score/command/Dispatchers/CommandDispatcher.hpp>
#include <SEGMent/Commands/CommandFactory.hpp>
#include <QDir>
#include <QGraphicsSceneMouseEvent>
#include <QPen>
#include <QBrush>
#include <QMimeData>
#include <QDebug>
#include <wobjectimpl.h>
W_OBJECT_IMPL(SEGMent::RectItem)

namespace SEGMent
{

RectItem::RectItem(
    qreal x,
    qreal y,
    qreal w,
    qreal h,
    bool constrainPosToParent,
      const score::DocumentContext& ctx,
            ZoomView& view,
    QGraphicsItem* parent)
    : QGraphicsRectItem(x, y, w, h, parent)
    , context{ctx}
    , m_view{view}
    , m_posConstrainedToParent{constrainPosToParent}
{
  setPen(score::Skin::instance().TransparentPen);
  setBrush(score::Skin::instance().TransparentBrush);
  setCacheMode(QGraphicsItem::DeviceCoordinateCache);

  setAcceptDrops(false);
}

void RectItem::setMinSize(qreal minWidth, qreal minHeight)
{
  m_minWidth = minWidth;
  m_minHeight = minHeight;
}

void RectItem::setRect(const QRectF& rect)
{
  qreal width = rect.width();
  qreal height = rect.height();

  if ((width < m_minWidth) || (height < m_minHeight))
  {
    return;
  }

  if (m_posConstrainedToParent)
  {
    if (pos().x() + width > parentItem()->boundingRect().width())
    {
      return;
    }

    if (pos().y() + height > parentItem()->boundingRect().height())
    {
      return;
    }
  }

  QGraphicsRectItem::setRect(rect);

  for (unsigned long i = 0; i < m_childWindows.size(); ++i)
  {
    m_childWindows[i]->updateRect();
  }

  on_sizeChanged();
}

void RectItem::setRect(qreal x, qreal y, qreal w, qreal h)
{
  RectItem::setRect({x, y, w, h});
}

void RectItem::dragMoveEvent(QGraphicsSceneDragDropEvent* event)
{
   m_view.dragMove(event->scenePos());
}

void RectItem::moveBy(qreal dx, qreal dy)
{
  QGraphicsRectItem::moveBy(dx, dy);
}

void RectItem::updateRect()
{

}

void RectItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
  event->ignore();
}

void RectItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
  event->ignore();
}

void RectItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
  event->ignore();
}

}
