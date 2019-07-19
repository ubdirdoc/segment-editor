#pragma once
#include <SEGMent/Items/Window.hpp>
#include <SEGMent/ImageCache.hpp>
namespace SEGMent
{
class ImageWindow;
class GifWindow;
class ClickWindow;
class BackClickWindow;
class TextWindow;
class ClickAreaModel;

//!
class HLineItem : public QGraphicsItem, public QObject
{
public:
  using QGraphicsItem::QGraphicsItem;
  void setLength(qreal l)
  {
    prepareGeometryChange();
    m_length = l;
    update();
  }
  QRectF boundingRect() const override
  {
    return QRectF{0., 0., m_length, 2.};
  }
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override
  {
    static const QPen& pen = Style::instance().sceneBorderPen;
    painter->setPen(pen);
    painter->drawLine(QLineF(1., 1., m_length, 1.));
  }
  qreal m_length{};
};
class VLineItem : public QGraphicsItem, public QObject
{
public:
  using QGraphicsItem::QGraphicsItem;
  void setHeight(qreal h)
  {
    prepareGeometryChange();
    m_height = h;
    update();
  }
  QRectF boundingRect() const override
  {
    return QRectF{0., 0., 2., m_height};
  }
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override
  {
    static const QPen& pen = Style::instance().sceneBorderPen;
    painter->setPen(pen);
    painter->drawLine(QLineF(1., 1., 1., m_height));
  }
  qreal m_height{};
};

//! Visual item corresponding to SceneModel
class SceneWindow : public Window
{
public:
  SceneWindow(
      const SceneModel& r,
      const score::DocumentContext& ctx,
      ZoomView& view,
      QGraphicsItem* parent);
  ~SceneWindow() override;

  static constexpr auto static_type() { return Qt::UserRole + 9986; }
  int type() const final override { return static_type(); }

  auto& childWindows() const { return m_childWindows; }
  const SceneModel& model() const { return m_scene; }

  void setTitle(QString title);
  void setBackgroundImage(CacheInstance& img);
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
  QVariant
  itemChange(GraphicsItemChange change, const QVariant& value) override;

  transition_t createTransitionFrom(
      anchor_id source_anchor,
      const SceneModel& target,
      anchor_id target_anchor) override;

  const SceneModel& m_scene;
  ZoomView& m_view;
  RectItem m_titleBar;
  QGraphicsProxyWidget m_titleProxy;
  QLineEditAutoResizeAutoCenter m_title;

  RectItem m_sceneArea;

  LODPixmapItem m_backgroundImgDisplay;

  HLineItem m_topLine{this};
  HLineItem m_bottomLine{this};
  VLineItem m_leftLine{this};
  VLineItem m_rightLine{this};

  qreal m_backgroundImgRealWidth{100.0};

  ossia::ptr_map<const ImageModel*, ImageWindow*> m_objects;
  ossia::ptr_map<const GifModel*, GifWindow*> m_gifs;
  ossia::ptr_map<const ClickAreaModel*, ClickWindow*> m_clickAreas;
  ossia::ptr_map<const BackClickAreaModel*, BackClickWindow*> m_backClickAreas;
  ossia::ptr_map<const TextAreaModel*, TextWindow*> m_textAreas;

  bool m_moving{false};
};
} // namespace SEGMent
