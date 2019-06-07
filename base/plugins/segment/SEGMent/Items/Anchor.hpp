#ifndef SEGMENTANCHOR_HPP
#define SEGMENTANCHOR_HPP

#include <QGraphicsEllipseItem>

#include <verdigris>

#include <vector>
class SEGMentArrow;

namespace SEGMent
{
using anchor_id = int8_t;
class ZoomView;

//! Graphical anchor used around objects when selecting them
class Anchor final : public QObject, public QGraphicsItem
{
  W_OBJECT(Anchor)
public:
  const anchor_id id;
  Anchor(qreal w, anchor_id id, QGraphicsItem* parent, ZoomView& view);

  void moved() W_SIGNAL(moved);

  QRectF boundingRect() const override;

private:
  ZoomView& m_processView;

  QPainterPath shape() const override;
  bool contains(const QPointF& point) const override;

  void dragEnterEvent(QGraphicsSceneDragDropEvent* event) override;
  void dropEvent(QGraphicsSceneDragDropEvent* event) override;
  void dragLeaveEvent(QGraphicsSceneDragDropEvent* event) override;

  void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

  QVariant
  itemChange(GraphicsItemChange change, const QVariant& value) override;

  void paint(
      QPainter* painter,
      const QStyleOptionGraphicsItem* option,
      QWidget* widget) override;

  qreal m_width{5.};
};
} // namespace SEGMent

#endif // SEGMENTANCHOR_HPP
