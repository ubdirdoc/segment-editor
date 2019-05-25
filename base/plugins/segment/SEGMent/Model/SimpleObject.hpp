#pragma once
#include <SEGMent/Model/Sound.hpp>
#include <SEGMent/Model/Image.hpp>
#include <SEGMent/Model/Traits.hpp>
#include <SEGMent/Model/Model.hpp>
#include <score/model/Entity.hpp>
#include <score/selection/Selectable.hpp>

namespace SEGMent
{
//! A simple object with an image. Can be associated to a transition.
class ImageModel
    : public Object<ImageModel>
{
  W_OBJECT(ImageModel)
  SCORE_SERIALIZE_FRIENDS
public:
  using base_type = Object<ImageModel>;
  Selectable selection{this};

  ImageModel(Id<ImageModel> id, QObject* parent);

  template <typename DeserializerVisitor>
  ImageModel(DeserializerVisitor&& vis, QObject* parent)
      : base_type{vis, parent}
  {
    vis.writeTo(*this);
  }

public:
  const Image& image() const MSVC_NOEXCEPT;
  void setImage(const Image& v) MSVC_NOEXCEPT;
  void imageChanged(Image v) W_SIGNAL(imageChanged, v);
  PROPERTY(Image, image READ image WRITE setImage NOTIFY imageChanged)
private:
  Image m_image;

public:
  bool puzzle() const MSVC_NOEXCEPT;
  void setPuzzle(bool v) MSVC_NOEXCEPT;
  void puzzleChanged(bool v) W_SIGNAL(puzzleChanged, v);
  PROPERTY(bool, puzzle READ puzzle WRITE setPuzzle NOTIFY puzzleChanged)
private:
  bool m_puzzle{false};
};

}


DEFAULT_MODEL_METADATA(SEGMent::ImageModel, "Object")
TR_TEXT_METADATA(, SEGMent::ImageModel, PrettyName_k, "Object")
