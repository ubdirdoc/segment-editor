#pragma once
#include <SEGMent/Items/Window.hpp>

namespace SEGMent
{
struct ObjectMover;
class ClickAreaModel;

//! The visual item corresponding to a ClickAreaModel
class ClickWindow : public Window
{
  friend struct ObjectMover;

public:
  ClickWindow(
      const ClickAreaModel& r,
      const score::DocumentContext& ctx,
      ZoomView& view,
      QGraphicsItem* parent);

  static constexpr auto static_type() { return Qt::UserRole + 9567; }
  int type() const final override { return static_type(); }

  const ClickAreaModel& model() const { return m_object; }

  void updateRect() override;
  void sizeChanged();

private:
  void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
  void paint(
      QPainter* painter,
      const QStyleOptionGraphicsItem* option,
      QWidget* widget) override;

  void dropEvent(QGraphicsSceneDragDropEvent* e) override;

  transition_t createTransitionFrom(
      anchor_id source_anchor,
      const SceneModel& target,
      anchor_id target_anchor) override;

  const ClickAreaModel& m_object;
};

class BackClickAreaModel;
//! The visual item corresponding to a BackClickAreaModel
class BackClickWindow : public Window
{
  friend struct ObjectMover;

public:
  BackClickWindow(
      const BackClickAreaModel& r,
      const score::DocumentContext& ctx,
      ZoomView& view,
      QGraphicsItem* parent);

  static constexpr auto static_type() { return Qt::UserRole + 9547; }
  int type() const final override { return static_type(); }

  const BackClickAreaModel& model() const { return m_object; }

  void updateRect() override;
  void sizeChanged();

private:
  void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
  void paint(
      QPainter* painter,
      const QStyleOptionGraphicsItem* option,
      QWidget* widget) override;

  void dropEvent(QGraphicsSceneDragDropEvent* e) override;

  transition_t createTransitionFrom(
      anchor_id source_anchor,
      const SceneModel& target,
      anchor_id target_anchor) override;

  const BackClickAreaModel& m_object;
  bool m_selection{false};
};

//! The visual item corresponding to a TextAreaModel
class TextWindow : public Window
{
  friend struct ObjectMover;

public:
  TextWindow(
      const TextAreaModel& r,
      const score::DocumentContext& ctx,
      ZoomView& view,
      QGraphicsItem* parent);

  static constexpr auto static_type() { return Qt::UserRole + 9537; }

  int type() const final override;

  const TextAreaModel& model() const;

  void updateRect() override;
  void sizeChanged();

private:
  void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
  void paint(
      QPainter* painter,
      const QStyleOptionGraphicsItem* option,
      QWidget* widget) override;

  void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;
  void dropEvent(QGraphicsSceneDragDropEvent* e) override;

  void setText(const QString& str);
  void updateFont();
  transition_t createTransitionFrom(
      anchor_id source_anchor,
      const SceneModel& target,
      anchor_id target_anchor) override;

  const TextAreaModel& m_object;
  bool m_selection{false};
  QFont m_font;
  QString m_text;
};

} // namespace SEGMent
