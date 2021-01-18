#pragma once
#include <SEGMent/Model/Scene.hpp>
#include <SEGMent/Model/ClickArea.hpp>
#include <SEGMent/Model/BackClickArea.hpp>
#include <SEGMent/Model/TextArea.hpp>
#include <SEGMent/Model/GifObject.hpp>
#include <SEGMent/Model/Transition.hpp>
#include <tuple>

namespace SEGMent
{
using SceneChildrenTypes = std::tuple<
  ImageModel,
  GifModel,
  ClickAreaModel,
  BackClickAreaModel,
  TextAreaModel
>;

struct ImageWindow;
struct GifWindow;
struct ClickWindow;
struct BackClickWindow;
struct TextWindow;

template <typename T>
struct SceneAccessor;
template <>
struct SceneAccessor<ImageModel>
{
  using view = ImageWindow;
  static auto& get(SceneModel& scene) { return scene.objects; }
  static auto& get(const SceneModel& scene) { return scene.objects; }
};
template <>
struct SceneAccessor<GifModel>
{
  using view = GifWindow;
  static auto& get(SceneModel& scene) { return scene.gifs; }
  static auto& get(const SceneModel& scene) { return scene.gifs; }
};
template <>
struct SceneAccessor<ClickAreaModel>
{
  using view = ClickWindow;
  static auto& get(SceneModel& scene) { return scene.clickAreas; }
  static auto& get(const SceneModel& scene) { return scene.clickAreas; }
};
template <>
struct SceneAccessor<BackClickAreaModel>
{
  using view = BackClickWindow;
  static auto& get(SceneModel& scene) { return scene.backClickAreas; }
  static auto& get(const SceneModel& scene) { return scene.backClickAreas; }
};
template <>
struct SceneAccessor<TextAreaModel>
{
  using view = TextWindow;
  static auto& get(SceneModel& scene) { return scene.textAreas; }
  static auto& get(const SceneModel& scene) { return scene.textAreas; }
};
}
