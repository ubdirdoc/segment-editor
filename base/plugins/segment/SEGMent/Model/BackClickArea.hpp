#pragma once
#include <score/model/Entity.hpp>
#include <score/selection/Selectable.hpp>

#include <SEGMent/Model/Model.hpp>
#include <SEGMent/Model/Sound.hpp>
#include <SEGMent/Model/Traits.hpp>
namespace SEGMent
{
/**
 * @brief Data model for the back click zones.
 * Does not contain any particular data.
 */
class BackClickAreaModel : public Object<BackClickAreaModel>
{
  W_OBJECT(BackClickAreaModel)
  SCORE_SERIALIZE_FRIENDS
public:
  using base_type = Object<BackClickAreaModel>;
  Selectable selection{this};

  BackClickAreaModel(const Id<BackClickAreaModel>& id, QObject* parent);

  template <typename DeserializerVisitor>
  BackClickAreaModel(DeserializerVisitor&& vis, QObject* parent)
      : base_type{std::forward<DeserializerVisitor>(vis), parent}
  {
    vis.writeTo(*this);
  }
};
} // namespace SEGMent

DEFAULT_MODEL_METADATA(SEGMent::BackClickAreaModel, "BackClick")
TR_TEXT_METADATA(
    ,
    SEGMent::BackClickAreaModel,
    PrettyName_k,
    "Back-Click Area")
