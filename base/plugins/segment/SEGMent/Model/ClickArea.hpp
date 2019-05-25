#pragma once
#include <SEGMent/Model/Sound.hpp>
#include <SEGMent/Model/Traits.hpp>
#include <SEGMent/Model/Model.hpp>
#include <score/model/Entity.hpp>
#include <score/selection/Selectable.hpp>
namespace SEGMent
{

class ClickAreaModel
    : public Object<ClickAreaModel>
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
}

DEFAULT_MODEL_METADATA(SEGMent::ClickAreaModel, "Click")
TR_TEXT_METADATA(, SEGMent::ClickAreaModel, PrettyName_k, "Click Area")
