#pragma once
#include <score/model/EntityMap.hpp>
#include <score/tools/Metadata.hpp>

#include <SEGMent/Model/Scene.hpp>
#include <SEGMent/Model/Transition.hpp>
namespace SEGMent
{
/**
 * @brief The actual root of a SEGMent document.
 *
 * Contains a set of scenes and a set of transitions.
 */
class ProcessModel final : public score::Entity<ProcessModel>
{
  SCORE_SERIALIZE_FRIENDS

public:
  ProcessModel(Id<ProcessModel> id, QObject* parent);

  template <typename Impl>
  ProcessModel(Impl&& vis, QObject* parent)
      : score::Entity<ProcessModel>{vis, parent}
  {
    vis.writeTo(*this);
  }

  ~ProcessModel();

  score::EntityMap<SceneModel> scenes;
  score::EntityMap<TransitionModel> transitions;
};
} // namespace SEGMent

OBJECTKEY_METADATA(, SEGMent::ProcessModel, "SEGMentProcess")
TR_TEXT_METADATA(, SEGMent::ProcessModel, PrettyName_k, "Process")
