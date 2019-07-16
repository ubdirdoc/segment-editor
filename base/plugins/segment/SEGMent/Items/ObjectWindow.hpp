#pragma once
#include <SEGMent/Items/Window.hpp>
#include <SEGMent/ImageCache.hpp>
namespace SEGMent
{

//! A window which has a background image.
template<typename ImageType>
class WindowWithBackground : public Window
{
public:
  template <typename... Args>
  WindowWithBackground(Args&&... args) : Window{args...}
  {
    m_backgroundImgDisplay.setParentItem(this);
  }

  void sizeChanged()
  {
    m_backgroundImgDisplay.setScale(
        boundingRect().width() / (m_backgroundImgRealWidth));

    m_anchorsSetter.updateAnchorsPos();

    if (m_sizeGripItem)
      m_sizeGripItem->reset();
  }

  void setImageOpacity(qreal opacity)
  {
    m_backgroundImgDisplay.setOpacity(opacity);
  }

protected:
  ImageType m_backgroundImgDisplay;
  qreal m_backgroundImgRealWidth{100.0};
  qreal m_backgroundImgRealHeight{100.0};
};

//! A window which can be resized manually.
template <typename T, typename ImageType>
class ResizableWindow : public WindowWithBackground<ImageType>
{
public:
  using WindowWithBackground<ImageType>::WindowWithBackground;

  void updateRect() override
  {
    auto& self = static_cast<T&>(*this);
    ObjectMover::updateRect(self, self.model().pos(), self.model().size());
  }
};

//! Visual item for ImageModel
class ImageWindow : public ResizableWindow<ImageWindow, LODPixmapItem>
{
public:
  ImageWindow(
      const ImageModel& r,
      const score::DocumentContext& ctx,
      ZoomView& view,
      QGraphicsItem* parent);

  static constexpr auto static_type() { return Qt::UserRole + 9987; }
  int type() const final override { return static_type(); }

  const ImageModel& model() const { return m_object; }

private:
  void setPuzzle(bool b);
  void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
  void dropEvent(QGraphicsSceneDragDropEvent* e) override;

  transition_t createTransitionFrom(
      anchor_id source_anchor,
      const SceneModel& target,
      anchor_id target_anchor) override;

  const ImageModel& m_object;
  QGraphicsPixmapItem* m_puzzle{};
};

//! Visual item for GifModel
class GifWindow : public ResizableWindow<GifWindow, QGraphicsPixmapItem>
{
public:
  GifWindow(
      const GifModel& r,
      const score::DocumentContext& ctx,
      ZoomView& view,
      QGraphicsItem* parent);

  static constexpr auto static_type() { return Qt::UserRole + 9984; }
  int type() const final override { return static_type(); }

  const GifModel& model() const { return m_object; }

private:
  void paint(
      QPainter* painter,
      const QStyleOptionGraphicsItem* option,
      QWidget* widget) override;
  void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
  void dropEvent(QGraphicsSceneDragDropEvent* e) override;

  transition_t createTransitionFrom(
      anchor_id source_anchor,
      const SceneModel& target,
      anchor_id target_anchor) override;

  const GifModel& m_object;
  bool m_selection{false};
};
} // namespace SEGMent
