#pragma once
#include <ossia/detail/typelist.hpp>
#include <SEGMent/Model/Scene.hpp>
#include <SEGMent/Model/ClickArea.hpp>
#include <SEGMent/Model/BackClickArea.hpp>
#include <SEGMent/Model/TextArea.hpp>
#include <SEGMent/Model/GifObject.hpp>
#include <SEGMent/Model/Transition.hpp>

#include <SEGMent/Items/ClickWindow.hpp>
#include <SEGMent/Items/ObjectWindow.hpp>
#include <SEGMent/Items/SceneWindow.hpp>

namespace SEGMent
{
//! Used to run a function for every data model type available in SEGMent
template <typename F>
auto dispatch(const QObject* obj, F&& fun)
{
  if (auto v = qobject_cast<const SceneModel*>(obj))
    return fun(*v);
  if (auto v = qobject_cast<const ImageModel*>(obj))
    return fun(*v);
  if (auto v = qobject_cast<const GifModel*>(obj))
    return fun(*v);
  if (auto v = qobject_cast<const ClickAreaModel*>(obj))
    return fun(*v);
  if (auto v = qobject_cast<const BackClickAreaModel*>(obj))
    return fun(*v);
  if (auto v = qobject_cast<const TextAreaModel*>(obj))
    return fun(*v);
  if (auto v = qobject_cast<const TransitionModel*>(obj))
    return fun(*v);

  if constexpr (!std::is_same_v<
                    decltype(fun(std::declval<const SceneModel&>())),
                    void>)
    return decltype(fun(std::declval<const SceneModel&>())){};
}

//! Used to run a function for every item window type available in SEGMent
template <typename F>
auto dispatchItem(const QGraphicsItem* obj, F&& fun)
{
  if (auto v = dynamic_cast<const SceneWindow*>(obj))
    return fun(v->model());
  if (auto v = dynamic_cast<const ImageWindow*>(obj))
    return fun(v->model());
  if (auto v = dynamic_cast<const GifWindow*>(obj))
    return fun(v->model());
  if (auto v = dynamic_cast<const ClickWindow*>(obj))
    return fun(v->model());
  if (auto v = dynamic_cast<const BackClickWindow*>(obj))
    return fun(v->model());
  if (auto v = dynamic_cast<const TextWindow*>(obj))
    return fun(v->model());

  return decltype(fun(std::declval<const SceneModel&>())){};
}

//! Used to dispatch all the child elements of a scene to a functor
template <typename F>
auto dispatchSceneChildren(const SceneModel& scene, F&& fun)
{
  for(auto& o : scene.objects)
    fun(o);
  for(auto& o : scene.gifs)
    fun(o);
  for(auto& o : scene.textAreas)
    fun(o);
  for(auto& o : scene.clickAreas)
    fun(o);
  for(auto& o : scene.backClickAreas)
    fun(o);
}

template <typename Scene_T, typename F>
auto forEachCategoryInScene(Scene_T& v, F&& func)
{
  const auto& objects = std::tie(
              v.objects,
              v.gifs,
              v.clickAreas,
              v.backClickAreas,
              v.textAreas
  );

  ossia::for_each_in_tuple(objects, std::forward<F>(func));
}
}
