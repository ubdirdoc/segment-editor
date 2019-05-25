#pragma once
#include <SEGMent/Model/Sound.hpp>
#include <SEGMent/Model/Traits.hpp>
#include <SEGMent/Model/Model.hpp>
#include <SEGMent/Model/Image.hpp>
#include <score/model/Entity.hpp>
#include <score/selection/Selectable.hpp>
#include <QMovie>
namespace SEGMent
{

/** An object which corresponds to a gif.
 *
 * Gifs can have multiple frames.
 *   - Multiple frames can be marked as "stops".
 *   - A single frame can be marked as "victorious".
 *   - If all the gifs in a scene are "victorious" then there is a transition
 *   to the next scene.
 */
struct GifModel
    : public Object<GifModel>
{
  W_OBJECT(GifModel)
  SCORE_SERIALIZE_FRIENDS
public:
  enum Frame { Ignored, Active, Victorious };
  using Frames = std::vector<Frame>;
  using base_type = Object<GifModel>;
  Selectable selection{this};

  GifModel(const Id<GifModel>& id, QObject* parent);

  template <typename DeserializerVisitor>
  GifModel(DeserializerVisitor&& vis, QObject* parent)
      : base_type{vis, parent}
  {
    vis.writeTo(*this);
  }

  mutable QMovie gif;
public:
  const Image& image() const MSVC_NOEXCEPT;
  void setImage(const Image& v) MSVC_NOEXCEPT;
  void imageChanged(const Image& v) W_SIGNAL(imageChanged, v);
  PROPERTY(Image, image READ image WRITE setImage NOTIFY imageChanged)

public:
  const Frames& frames() const MSVC_NOEXCEPT;
  void setFrames(const Frames& v) MSVC_NOEXCEPT;
  void framesChanged(const Frames& v) W_SIGNAL(framesChanged, v);
  PROPERTY(Frames, frames READ frames WRITE setFrames NOTIFY framesChanged)

public:
  int defaultFrame() const MSVC_NOEXCEPT;
  void setDefaultFrame(int v) MSVC_NOEXCEPT;
  void defaultFrameChanged(int v) W_SIGNAL(defaultFrameChanged, v);
  PROPERTY(int, defaultFrame READ defaultFrame WRITE setDefaultFrame NOTIFY defaultFrameChanged)

private:
  Image m_image;
  Frames m_frames;
  int m_defaultFrame{};
};

}
Q_DECLARE_METATYPE(SEGMent::GifModel::Frames)
W_REGISTER_ARGTYPE(SEGMent::GifModel::Frames)
DEFAULT_MODEL_METADATA(SEGMent::GifModel, "Gif")
TR_TEXT_METADATA(, SEGMent::GifModel, PrettyName_k, "Gif")
