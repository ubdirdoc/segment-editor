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
 * @brief Run an action on every file referenced in a SEGMent scene.
 *
 * It is used to copy only the actually used files to the right places.
 */
struct ExportVisitor
{
  std::function<void(const QString&)> copyFile;
  void operator()(const SceneModel& scene)
  {
    copyFile(scene.image().path);
    if(!scene.ambience().path().isEmpty())
      copyFile(scene.ambience().path());

    dispatchSceneChildren(scene, *this);
  }

  void operator()(const TransitionModel& obj)
  {
    if(!obj.sound().path().isEmpty())
      copyFile(obj.sound().path());
  }

  void operator()(const GifModel& obj)
  {
    copyFile(obj.image().path);
    if(!obj.sound().path().isEmpty())
      copyFile(obj.sound().path());
  }
  void operator()(const ImageModel& obj)
  {
    copyFile(obj.image().path);
    if(!obj.sound().path().isEmpty())
      copyFile(obj.sound().path());
  }
  void operator()(const TextAreaModel& obj)
  {
    if(!obj.sound().path().isEmpty())
      copyFile(obj.sound().path());
  }
  void operator()(const ClickAreaModel& obj)
  {
    if(!obj.sound().path().isEmpty())
      copyFile(obj.sound().path());
  }
  void operator()(const BackClickAreaModel& obj)
  {
    if(!obj.sound().path().isEmpty())
      copyFile(obj.sound().path());
  }

};
}
