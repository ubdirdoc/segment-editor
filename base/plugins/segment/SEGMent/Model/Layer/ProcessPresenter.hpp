#pragma once
#include <QPoint>
#include <score/model/Identifier.hpp>
#include <SEGMent/Model/Scene.hpp>
#include <SEGMent/Items/Window.hpp>
#include <SEGMent/Items/Arrow.hpp>
#include <SEGMent/Model/Transition.hpp>
#include <ossia/detail/ptr_set.hpp>

namespace SEGMent
{
class ProcessModel;
class ZoomView;

//! Shows the element of a SEGMent ProcessModel in a View
class Presenter final
  : public QObject
  , public Nano::Observer
{
public:
  explicit Presenter(
      const ProcessModel& model,
      ZoomView* view,
      const score::DocumentContext& ctx,
      QObject* parent);

  void on_sceneCreated(const SceneModel&);
  void on_sceneRemoved(const SceneModel&);
  void on_transitionCreated(const TransitionModel&);
  void on_transitionRemoved(const TransitionModel&);

private:
  const score::DocumentContext& context;
  ZoomView& m_view;

  ossia::ptr_map<const SceneModel*, SceneWindow*> m_scenes;
  ossia::ptr_map<const TransitionModel*, Arrow*> m_transitions;
};
}
