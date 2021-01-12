#pragma once
#include <SEGMent/Model/Scene.hpp>
#include <SEGMent/Model/ClickArea.hpp>
#include <SEGMent/Model/BackClickArea.hpp>
#include <SEGMent/Model/TextArea.hpp>
#include <SEGMent/Model/GifObject.hpp>
#include <SEGMent/Model/Transition.hpp>

#include <SEGMent/Visitors.hpp>

namespace SEGMent
{

/**
 * @brief Lists all the ressources of a given selection inside a JSON array.
 *
 * This is used for copy-paste across multiple SEGMent editor processes.
 */
struct ResourceVisitor
{
  QJsonArray resources;
  void operator()(const SceneModel& scene)
  {
    resources.push_back(scene.image().path);
    if(!scene.ambience().path().isEmpty())
      resources.push_back(scene.ambience().path());

    dispatchSceneChildren(scene, *this);
  }

  void operator()(const TransitionModel& obj)
  {

  }

  void operator()(const GifModel& obj)
  {
    resources.push_back(obj.image().path);
    if(!obj.sound().path().isEmpty())
      resources.push_back(obj.sound().path());
  }
  void operator()(const ImageModel& obj)
  {
    resources.push_back(obj.image().path);
    if(!obj.sound().path().isEmpty())
      resources.push_back(obj.sound().path());
  }
  void operator()(const TextAreaModel& obj)
  {
    if(!obj.sound().path().isEmpty())
      resources.push_back(obj.sound().path());
  }
  void operator()(const ClickAreaModel& obj)
  {
    if(!obj.sound().path().isEmpty())
      resources.push_back(obj.sound().path());
  }
  void operator()(const BackClickAreaModel& obj)
  {
    if(!obj.sound().path().isEmpty())
      resources.push_back(obj.sound().path());
  }
};

/**
 * @brief Creates the json data necessary to copy
 */
struct CopyObjectVisitor
{
  QJsonArray scenes;
  QJsonArray transitions;

  QJsonArray objects;

  ResourceVisitor resources;

  void operator()(const SceneModel& obj)
  {
    auto json = score::marshall<JSONObject>(obj);
    scenes.push_back(json);
    resources(obj);
  }

  void operator()(const TransitionModel& obj)
  {

  }

  template<typename T>
  void operator()(const T& obj)
  {
    auto json = score::marshall<JSONObject>(obj);
    json["ObjectKind"] = Metadata<ObjectKey_k, T>::get();
    objects.push_back(json);

    resources(obj);
  }
};


}
