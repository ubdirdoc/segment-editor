#include <SEGMent/Items/RectItem.hpp>
#include <SEGMent/Items/Window.hpp>
#include <SEGMent/Items/ObjectWindow.hpp>
#include <SEGMent/Model/Layer/ProcessView.hpp>
#include <SEGMent/Commands/CommandFactory.hpp>
#include <score/command/Dispatchers/CommandDispatcher.hpp>
#include <wobjectimpl.h>
W_OBJECT_IMPL(SEGMent::Window)

namespace SEGMent
{

Window::Window(
    QPointF pos,
    QSizeF sz,
    bool constrainPosToParent,
    bool anchors,
    const score::DocumentContext& ctx,
    QGraphicsItem* parent):
  RectItem{
    0, 0, sz.width(), sz.height(), constrainPosToParent, this, ctx, parent}
, m_anchorsSetter{anchors ? 3 : 0, (borderWidth * 3) - 1, *this}
{
  this->setBrush(score::Skin::instance().TransparentBrush);
  this->moveBy(pos.x(), pos.y());
  this->setFlag(QGraphicsItem::ItemIsMovable);

  setMinSize(20, 20);
}

Anchor* Window::anchor(anchor_id id) const
{
  return m_anchorsSetter.anchor(id);
}

}
