#ifndef SEGMENTARROW_HPP
#define SEGMENTARROW_HPP

#include <QGraphicsLineItem>
#include <SEGMent/Model/Layer/ProcessView.hpp>

namespace SEGMent
{
class Anchor;
//! The visual item corresponding to a TransitionModel
class Arrow final
    : public QObject
    , public QGraphicsLineItem
{
public:
  Arrow(
      const TransitionModel& trans,
      Anchor& startItem,
      Anchor& endItem,
      const score::DocumentContext& ctx,
      ZoomView& view,
      QGraphicsItem* parentItem = 0);

  Anchor& startItem() const
  {
    return m_startItem;
  }

  Anchor& endItem() const
  {
    return m_endItem;
  }

  enum
  {
    Type = UserType + 4
  };

  int type() const override
  {
    return Type;
  }

  const score::DocumentContext& context;
private:
  void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

  void dragEnterEvent(QGraphicsSceneDragDropEvent* event) override;
  void dragMoveEvent(QGraphicsSceneDragDropEvent* event) override;
  void dragLeaveEvent(QGraphicsSceneDragDropEvent* event) override;
  void dropEvent(QGraphicsSceneDragDropEvent* event) override;

  void setSelected(bool b);
  void updateShape();

  QRectF boundingRect() const override;
  QPainterPath shape() const override;
  void paint(
      QPainter* painter,
      const QStyleOptionGraphicsItem* option,
      QWidget* widget = 0) override;

  const TransitionModel& m_model;
  Anchor& m_startItem;
  Anchor& m_endItem;
  ZoomView& m_processView;

  QPolygonF m_arrowHead;
  QPolygonF m_selectionPolygon;

  bool m_selected{};
  bool m_dragging{};
};
}

#endif // SEGMENTARROW_HPP
