#include <SEGMent/Model/Layer/ProcessPresenter.hpp>
#include <SEGMent/Model/Layer/ProcessView.hpp>
#include <SEGMent/Model/ProcessModel.hpp>
#include <SEGMent/Items/ObjectWindow.hpp>
#include <SEGMent/Items/SceneWindow.hpp>
#include <SEGMent/Items/ClickWindow.hpp>
#include <SEGMent/Items/RectItem.hpp>
#include <SEGMent/Items/Arrow.hpp>
#include <score/command/Dispatchers/CommandDispatcher.hpp>
#include <ossia/detail/apply.hpp>

namespace SEGMent
{
Presenter::Presenter(
    const ProcessModel& layer,
    ZoomView* view,
    const score::DocumentContext& ctx,
    QObject* parent)
    : QObject{parent}
    , context{ctx}
    , m_view{*view}
{
  const SEGMent::ProcessModel& p = layer;

  for (const auto& scene : p.scenes)
  {
    on_sceneCreated(scene);
  }

  p.scenes.added.connect<&Presenter::on_sceneCreated>(this);
  p.scenes.removed.connect<&Presenter::on_sceneRemoved>(this);

  for (const auto& trans : p.transitions)
  {
    on_transitionCreated(trans);
  }

  p.transitions.added.connect<&Presenter::on_transitionCreated>(this);
  p.transitions.removed.connect<&Presenter::on_transitionRemoved>(this);
}

void Presenter::on_sceneCreated(const SceneModel& scene)
{
  auto sc = new SceneWindow(scene, context, m_view, nullptr);
  m_scenes.insert({&scene, sc});
  m_view.addScene(sc);
}

void Presenter::on_sceneRemoved(const SceneModel& scene)
{
  auto it = m_scenes.find(&scene);
  if(it != m_scenes.end())
  {
    m_view.removeScene(it->second);
    m_scenes.erase(it);
  }
}

void Presenter::on_transitionCreated(const TransitionModel& transition)
{
  // Find the start and end anchors of the transitions

  struct tm {
      using ret = std::tuple<Anchor*, Anchor*>;
      const Presenter& self;
    ret operator()(const SceneToScene& t) const
    {
      auto it1 = self.m_scenes.find(t.from.try_find(self.context));
      auto it2 = self.m_scenes.find(t.to.try_find(self.context));
      if(it1 == self.m_scenes.end() || it2 == self.m_scenes.end())
        return {nullptr, nullptr};
      return {it1->second->anchor(t.source), it2->second->anchor(t.target)};
    }
    ret operator()(const ObjectToScene& t) const
    {
      auto obj = t.from.try_find(self.context);
      auto obj_item = self.m_view.findItem<ImageWindow>(obj);

      auto it2 = self.m_scenes.find(t.to.try_find(self.context));
      if(!obj_item || it2 == self.m_scenes.end())
        return {nullptr, nullptr};
      return {obj_item->anchor(t.source), it2->second->anchor(t.target)};
    }
    ret operator()(const GifToScene& t) const
    {
      return {nullptr, nullptr};
    }
    ret operator()(const ClickToScene& t) const
    {
      auto obj = t.from.try_find(self.context);
      auto obj_item = self.m_view.findItem<ClickWindow>(obj);

      auto it2 = self.m_scenes.find(t.to.try_find(self.context));
      if(!obj_item || it2 == self.m_scenes.end())
        return {nullptr, nullptr};
      return {obj_item->anchor(t.source), it2->second->anchor(t.target)};
    }
    ret operator()() const
    {
      return {nullptr, nullptr};
    }
  } transition_matcher{*this};

  auto [start, end] = ossia::apply(transition_matcher, transition.transition());
  if(start && end)
  {
    auto t = new Arrow(transition, *start, *end, context, m_view, nullptr);
    m_transitions.insert({&transition, t});
    m_view.addTransition(t);
  }
}
void Presenter::on_transitionRemoved(const TransitionModel& transition)
{
  auto it = m_transitions.find(&transition);
  if(it != m_transitions.end())
  {
    m_view.removeTransition(it->second);
    m_transitions.erase(it);
  }
}


}
