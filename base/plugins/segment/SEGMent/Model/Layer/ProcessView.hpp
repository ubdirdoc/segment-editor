#pragma once
#include <QGraphicsView>
#include <SEGMent/Items/GlobalVariables.hpp>
#include <SEGMent/Items/SceneWindow.hpp>
#include <SEGMent/Model/SimpleObject.hpp>
#include <SEGMent/Model/ProcessModel.hpp>

namespace SEGMent
{
class Arrow;

//! This class iterates on all scenes to find a child object which matches with a model.
//! TODO if this becomes a performance bottleneck one day, replace this by a hash_map<model*, view*>
template<typename Model_T, typename View_T>
struct window_visitor
{
    Model_T& request;
    Window* operator()(SceneWindow* parent)
    {
      for(auto cld : parent->childWindows())
      {
        if(cld->type() == View_T::static_type())
        {
          auto obj = static_cast<View_T*>(cld);
          if(&obj->model() == &request)
            return obj;
        }
      }
      return nullptr;
    }
};

//! SEGMent Canvas. Receives scenes drops and handles arrow creation
class View final
    : public QObject
    , public QGraphicsItem
{
  W_OBJECT(View)
  friend class RectItem;
  friend class Anchor;
public:
  explicit View(const ProcessModel& proc, const score::DocumentContext&, QGraphicsItem* parent);

  static constexpr auto static_type() noexcept
  { return UserType + 1579; }

  int type() const override
  { return static_type(); }

  void setStartAnchorForNewArrow(Anchor& startAnchor);
  void setEndAnchorForNewArrow(Anchor& endAnchor);
  void finishArrowDrop();

  Window* findItem(ImageModel* obj);

  void addScene(SceneWindow* s);

  void removeScene(const SceneWindow* s);
  void addTransition(Arrow* a);
  void removeTransition(const Arrow* a);

  void doubleClicked() W_SIGNAL(doubleClicked);

  template<typename View, typename Model>
  Window* findItem(Model* obj)
  {
    if(!obj)
      return nullptr;

    for(auto v : m_sceneWindows)
    {
      if(auto p = window_visitor<Model, View>{*obj}(v))
        return p;
    }
    return nullptr;
  }

private:
  void paint(
      QPainter* painter,
      const QStyleOptionGraphicsItem* option,
      QWidget* widget) final override;

  QRectF boundingRect() const override
  { return { 0., 0., 500000., 500000. }; }

  void dropEvent(QGraphicsSceneDragDropEvent* event) override;
  void dragMoveEvent(QGraphicsSceneDragDropEvent* event) override;

  const ProcessModel& m_process;
  const score::DocumentContext& m_ctx;

  std::vector<SceneWindow*> m_sceneWindows;
  std::vector<SEGMent::Arrow*> m_sceneArrows;
  Anchor* m_startAnchorForNewArrow{};

  QGraphicsLineItem* m_tmpArrow{};
};

View* GetParentSEGMentView(QGraphicsItem* object);
}
