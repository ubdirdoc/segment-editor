#pragma once
#include <SEGMent/Model/Sound.hpp>
#include <SEGMent/Model/Traits.hpp>
#include <SEGMent/Model/Model.hpp>
#include <score/model/Entity.hpp>
#include <score/selection/Selectable.hpp>
namespace SEGMent
{
class BackClickAreaModel
    : public Object<BackClickAreaModel>
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
}

DEFAULT_MODEL_METADATA(SEGMent::BackClickAreaModel, "BackClick")
TR_TEXT_METADATA(, SEGMent::BackClickAreaModel, PrettyName_k, "Back-Click Area")
