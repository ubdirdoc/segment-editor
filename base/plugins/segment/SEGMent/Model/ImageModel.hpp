#pragma once
#include <score/model/Entity.hpp>
#include <score/selection/Selectable.hpp>

#include <SEGMent/Model/Image.hpp>
#include <SEGMent/Model/Model.hpp>
#include <SEGMent/Model/Sound.hpp>
#include <SEGMent/Model/Traits.hpp>

namespace SEGMent
{

/**
 * @brief Data model for the simple images.
 *
 * Can be associated to a transition.
 */
class ImageModel : public Object<ImageModel>
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

  // The image file used
public:
  const Image& image() const MSVC_NOEXCEPT;
  void setImage(const Image& v) MSVC_NOEXCEPT;
  void imageChanged(Image v) W_SIGNAL(imageChanged, v);
  PROPERTY(Image, image READ image WRITE setImage NOTIFY imageChanged)
private:
  Image m_image;

  // Is the image a piece of a puzzle
public:
  bool puzzle() const MSVC_NOEXCEPT;
  void setPuzzle(bool v) MSVC_NOEXCEPT;
  void puzzleChanged(bool v) W_SIGNAL(puzzleChanged, v);
  PROPERTY(bool, puzzle READ puzzle WRITE setPuzzle NOTIFY puzzleChanged)
private:
  bool m_puzzle{false};

public:
  const QString& eventRequired() const MSVC_NOEXCEPT;
  void setEventRequired(const QString& v) MSVC_NOEXCEPT;
  void eventRequiredChanged(const QString& v) W_SIGNAL(eventRequiredChanged, v);
  PROPERTY(QString, eventRequired READ eventRequired WRITE setEventRequired NOTIFY eventRequiredChanged)
private:
  QString m_eventRequired{};
};

} // namespace SEGMent

DEFAULT_MODEL_METADATA(SEGMent::ImageModel, "Object")
TR_TEXT_METADATA(, SEGMent::ImageModel, PrettyName_k, "Object")
