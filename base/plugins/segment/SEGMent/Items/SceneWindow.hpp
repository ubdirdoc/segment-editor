#pragma once
#include <SEGMent/Items/Window.hpp>
#include <SEGMent/ImageCache.hpp>
#include <SEGMent/Accessors.hpp>
#include <brigand/algorithms/transform.hpp>

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

  template<typename T>
  struct ChildWindowSet : public Nano::Observer
  {
    using accessor = ::SEGMent::SceneAccessor<T>;
    using model_type = T;
    using view_type = typename accessor::view;
    SceneWindow* parent{};
    ChildWindowSet()
    {
    }

    void init(SceneWindow& parent)
    {
      this->parent = &parent;

      auto& model_elements = accessor::get(parent.model());
      for (const auto& o : model_elements)
      {
        on_created(o);
      }

      model_elements.added.template connect<&ChildWindowSet::on_created>(this);
      model_elements.removed.template connect<&ChildWindowSet::on_removed>(this);
    }

    void on_created(const T& object)
    {
      auto sc = new view_type{object, parent->context, parent->m_view, &parent->m_sceneArea};
      parent->m_childWindows.push_back(sc);
      m_objects.insert({&object, sc});
    }

    void on_removed(const T& object)
    {
      auto it = m_objects.find(&object);
      if (it != m_objects.end())
      {
        auto ptr = it->second;
        {
          auto sit = ossia::find(parent->m_childWindows, ptr);
          if (sit != parent->m_childWindows.end())
          {
            parent->m_childWindows.erase(sit);
          }
        }

        m_objects.erase(it);
        delete ptr;
      }
    }

    ossia::ptr_map<const model_type*, view_type*> m_objects;
  };

  using items = brigand::transform<SEGMent::SceneChildrenTypes, brigand::bind<ChildWindowSet, brigand::_1>>;
  items m_items;
};
} // namespace SEGMent
