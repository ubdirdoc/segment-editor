#pragma once
#include <SEGMent/Items/Window.hpp>

namespace SEGMent
{
class ImageWindow;
class GifWindow;
class ClickWindow;
class BackClickWindow;
class TextWindow;
class ClickAreaModel;

//! Visual item corresponding to SceneModel
class SceneWindow
    : public Window
{
public:
  SceneWindow(const SceneModel& r,
              const score::DocumentContext& ctx,
              QGraphicsItem* parent);
  ~SceneWindow() override;

  static constexpr auto static_type() { return Qt::UserRole + 9986; }
  int type() const final override { return static_type(); }

  auto& childWindows() const { return m_childWindows; }
  const SceneModel& model() const { return m_scene; }

  void setTitle(QString title);
  void setBackgroundImage(QPixmap img);
  void updateRect() override;
  void sizeChanged();

  void dropEvent(QGraphicsSceneDragDropEvent* event) override;

  void on_objectCreated(const ImageModel& obj);
  void on_objectRemoved(const ImageModel& obj);
  void on_gifCreated(const GifModel& obj);
  void on_gifRemoved(const GifModel& obj);
  void on_clickAreaCreated(const ClickAreaModel& obj);
  void on_clickAreaRemoved(const ClickAreaModel& obj);
  void on_backClickAreaCreated(const BackClickAreaModel& obj);
  void on_backClickAreaRemoved(const BackClickAreaModel& obj);
  void on_textAreaCreated(const TextAreaModel& obj);
  void on_textAreaRemoved(const TextAreaModel& obj);

private:
  void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
  QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

  transition_t createTransitionFrom(anchor_id source_anchor, const SceneModel& target, anchor_id target_anchor) override;

  const SceneModel& m_scene;
  RectItem m_titleBar;
  QGraphicsProxyWidget m_titleProxy;
  QLineEditAutoResizeAutoCenter m_title;

  RectItem m_sceneArea;

  QGraphicsPixmapItem m_backgroundImgDisplay;
  qreal m_backgroundImgRealWidth{100.0};

  ossia::ptr_map<const ImageModel*, ImageWindow*> m_objects;
  ossia::ptr_map<const GifModel*, GifWindow*> m_gifs;
  ossia::ptr_map<const ClickAreaModel*, ClickWindow*> m_clickAreas;
  ossia::ptr_map<const BackClickAreaModel*, BackClickWindow*> m_backClickAreas;
  ossia::ptr_map<const TextAreaModel*, TextWindow*> m_textAreas;

  bool m_moving{false};
};
}
