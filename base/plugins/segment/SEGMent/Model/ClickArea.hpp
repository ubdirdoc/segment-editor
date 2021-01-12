#pragma once
#include <score/model/Entity.hpp>
#include <score/selection/Selectable.hpp>

#include <SEGMent/Model/Model.hpp>
#include <SEGMent/Model/Sound.hpp>
#include <SEGMent/Model/Traits.hpp>
namespace SEGMent
{
/**
 * @brief Data model for the click zones.
 * Does not contain any particular data.
 */
class ClickAreaModel : public Object<ClickAreaModel>
{
  W_OBJECT(ClickAreaModel)
  SCORE_SERIALIZE_FRIENDS
public:
  using base_type = Object<ClickAreaModel>;
  Selectable selection{this};

  ClickAreaModel(Id<ClickAreaModel> id, QObject* parent);

  template <typename DeserializerVisitor>
  ClickAreaModel(DeserializerVisitor&& vis, QObject* parent)
      : base_type{vis, parent}
  {
    vis.writeTo(*this);
  }
};
} // namespace SEGMent

DEFAULT_MODEL_METADATA(SEGMent::ClickAreaModel, "Click")
TR_TEXT_METADATA(, SEGMent::ClickAreaModel, PrettyName_k, "Click Area")
