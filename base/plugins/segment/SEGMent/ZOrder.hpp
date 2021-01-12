#pragma once
#include <score/document/DocumentContext.hpp>

#include <SEGMent/Model/Scene.hpp>
#include <SEGMent/Model/Transition.hpp>
#include <SEGMent/Commands/Creation.hpp>
#include <SEGMent/Commands/Deletion.hpp>

#include <score/command/Dispatchers/CommandDispatcher.hpp>

#include <limits>

namespace SEGMent
{
/**
 * @brief Computes the minimal and maximal z of the elements of a scene
 */
class SceneZOrders
{
public:
  template <typename T>
  SceneZOrders(const SceneModel& s, const T& self)
  {
    ossia::for_each_in_tuple(
        std::tie(
            s.objects, s.gifs, s.clickAreas, s.backClickAreas, s.textAreas),
        [&](const auto& map) {
          for (auto& obj : map)
          {
            using obj_t
                = std::remove_reference_t<std::remove_const_t<decltype(obj)>>;
            if constexpr (std::is_same_v<T, obj_t>)
            {
              if (&obj == &self)
                continue;
            }
            min = std::min(min, obj.z());
            max = std::max(max, obj.z());
            count++;
          }
        });
  }
  int min{INT_MAX}, max{0};
  int count = 0;
};

/**
 * @brief Action used to put an item at the front.
 */
struct MoveForwardVisitor
{
  const score::DocumentContext& context;
  void operator()(const SceneModel& obj) const {}

  void operator()(const TransitionModel& obj) const {}

  template <typename T>
  void operator()(const T& obj) const
  {
    using property = typename T::p_z;
    using cmd = typename score::PropertyCommand_T<property>::template command<
        void>::type;

    auto [min, max, count]
        = SceneZOrders{*safe_cast<SceneModel*>(obj.parent()), obj};
    if (count == 0)
      return;

    CommandDispatcher<>{context.commandStack}.submitCommand(
        new cmd{obj, std::min(obj.z() + 1, max + 1)});
  }
};

/**
 * @brief Action used to put an item at the back.
 */
struct MoveBackwardsVisitor
{
  const score::DocumentContext& context;
  void operator()(const SceneModel& obj) const {}

  void operator()(const TransitionModel& obj) const {}

  template <typename T>
  void operator()(const T& obj) const
  {
    using property = typename T::p_z;
    using cmd = typename score::PropertyCommand_T<property>::template command<
        void>::type;

    auto [min, max, count]
        = SceneZOrders{*safe_cast<SceneModel*>(obj.parent()), obj};
    if (count == 0)
      return;
    if (min <= 1)
      min = 1;

    CommandDispatcher<>{context.commandStack}.submitCommand(
        new cmd{obj, std::max(obj.z() - 1, min - 1)});
  }
};

}
