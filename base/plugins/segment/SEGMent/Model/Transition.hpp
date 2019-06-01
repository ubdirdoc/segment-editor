#pragma once
#include <score/model/Entity.hpp>
#include <score/model/path/PathSerialization.hpp>
#include <score/selection/Selectable.hpp>
#include <score/serialization/VariantSerialization.hpp>

#include <SEGMent/Items/Anchor.hpp>
#include <SEGMent/Model/ClickArea.hpp>
#include <SEGMent/Model/GifObject.hpp>
#include <SEGMent/Model/Model.hpp>
#include <SEGMent/Model/Riddle.hpp>
#include <SEGMent/Model/Scene.hpp>
#include <SEGMent/Model/SimpleObject.hpp>

namespace SEGMent
{
template <typename From, typename To>
struct Transition
{
  Transition() = default;
  Transition(const Transition&) = default;
  Transition(Transition&&) = default;
  Transition& operator=(const Transition&) = default;
  Transition& operator=(Transition&&) = default;
  Path<From> from;
  Path<To> to;
  anchor_id source{};
  anchor_id target{};

  bool operator==(const Transition& other) const MSVC_NOEXCEPT
  {
    return from == other.from && to == other.to && source == other.source
           && target == other.target;
  }
  bool operator!=(const Transition& other) const MSVC_NOEXCEPT
  {
    return !(*this == other);
  }
};

template <>
struct Transition<SceneModel, SceneModel>
{
  Transition() = default;
  Transition(const Transition&) = default;
  Transition(Transition&&) = default;
  Transition& operator=(const Transition&) = default;
  Transition& operator=(Transition&&) = default;
  Path<SceneModel> from;
  Path<SceneModel> to;
  anchor_id source{};
  anchor_id target{};

  riddle_t riddle;

  bool operator==(const Transition& other) const MSVC_NOEXCEPT
  {
    return from == other.from && to == other.to && source == other.source
           && target == other.target && riddle == other.riddle;
  }
  bool operator!=(const Transition& other) const MSVC_NOEXCEPT
  {
    return !(*this == other);
  }
};

using SceneToScene = Transition<SceneModel, SceneModel>;
using ObjectToScene = Transition<ImageModel, SceneModel>;
using GifToScene = Transition<GifModel, SceneModel>;
using ClickToScene = Transition<ClickAreaModel, SceneModel>;

using transition_t
    = eggs::variant<SceneToScene, ObjectToScene, GifToScene, ClickToScene>;

class TransitionModel : public score::Entity<TransitionModel>
{
public:
  W_OBJECT(TransitionModel)
  SCORE_SERIALIZE_FRIENDS

public:
  Selectable selection{this};

  TransitionModel(
      transition_t,
      const Id<TransitionModel>& id,
      QObject* parent);

  template <typename DeserializerVisitor>
  TransitionModel(DeserializerVisitor&& vis, QObject* parent)
      : Entity{vis, parent}
  {
    vis.writeTo(*this);
  }

  enum FadeMode
  {
    Immediate,
    Crossfade
  };

public:
  const transition_t& transition() const MSVC_NOEXCEPT;
  void setTransition(const transition_t& v) MSVC_NOEXCEPT;
  void transitionChanged(const transition_t& v) W_SIGNAL(transitionChanged, v);
  PROPERTY(
      transition_t,
      transition READ transition WRITE setTransition NOTIFY transitionChanged)
private:
  transition_t m_transition{};

public:
  const FadeMode& fade() const MSVC_NOEXCEPT;
  void setFade(const FadeMode& v) MSVC_NOEXCEPT;
  void fadeChanged(const FadeMode& v) W_SIGNAL(fadeChanged, v);
  PROPERTY(FadeMode, fade READ fade WRITE setFade NOTIFY fadeChanged)
private:
  FadeMode m_fade{FadeMode::Crossfade};

public:
  QColor color() const MSVC_NOEXCEPT;
  void setColor(QColor v) MSVC_NOEXCEPT;
  void colorChanged(QColor v) W_SIGNAL(colorChanged, v);
  PROPERTY(QColor, color READ color WRITE setColor NOTIFY colorChanged)
private:
  QColor m_color{};

public:
  const Sound& sound() const MSVC_NOEXCEPT;
  Sound& sound_ref() MSVC_NOEXCEPT;
  void setSound(const Sound& v) MSVC_NOEXCEPT;
  void soundChanged(const Sound& v) W_SIGNAL(soundChanged, v);
  PROPERTY(Sound, sound READ sound WRITE setSound NOTIFY soundChanged)
private:
  Sound m_sound{};
};

} // namespace SEGMent

JSON_METADATA(SEGMent::SceneToScene, "SceneToScene")
JSON_METADATA(SEGMent::ObjectToScene, "ObjectToScene")
JSON_METADATA(SEGMent::GifToScene, "GifToScene")
JSON_METADATA(SEGMent::ClickToScene, "ClickAreaToScene")

template <typename T, typename U>
struct TSerializer<DataStream, SEGMent::Transition<T, U>>
{
  static void
  readFrom(DataStream::Serializer& s, const SEGMent::Transition<T, U>& v)
  {
    auto& st = s.stream();
    st << v.from << v.to << v.source << v.target;
  }

  static void
  writeTo(DataStream::Deserializer& s, SEGMent::Transition<T, U>& v)
  {
    auto& st = s.stream();
    st >> v.from >> v.to >> v.source >> v.target;
  }
};

template <typename T, typename U>
struct TSerializer<JSONObject, SEGMent::Transition<T, U>>
{
  static void
  readFrom(JSONObject::Serializer& s, const SEGMent::Transition<T, U>& v)
  {
    s.obj["From"] = pathToString(v.from);
    s.obj["To"] = pathToString(v.to);
    s.obj["Source"] = v.source;
    s.obj["Target"] = v.target;
  }

  static void
  writeTo(JSONObject::Deserializer& s, SEGMent::Transition<T, U>& v)
  {
    v.from = SEGMent::pathFromString<T>(s.obj["From"].toString());
    v.to = SEGMent::pathFromString<U>(s.obj["To"].toString());
    v.source = s.obj["Source"].toInt();
    v.target = s.obj["Target"].toInt();
  }
};

template <>
struct TSerializer<
    DataStream,
    SEGMent::Transition<SEGMent::SceneModel, SEGMent::SceneModel>>
{
  using type = SEGMent::Transition<SEGMent::SceneModel, SEGMent::SceneModel>;
  static void readFrom(DataStream::Serializer& s, const type& v)
  {
    auto& st = s.stream();
    st << v.from << v.to << v.source << v.target << v.riddle;
  }

  static void writeTo(DataStream::Deserializer& s, type& v)
  {
    auto& st = s.stream();
    st >> v.from >> v.to >> v.source >> v.target >> v.riddle;
  }
};

template <>
struct TSerializer<
    JSONObject,
    SEGMent::Transition<SEGMent::SceneModel, SEGMent::SceneModel>>
{
  using type = SEGMent::Transition<SEGMent::SceneModel, SEGMent::SceneModel>;
  static void readFrom(JSONObject::Serializer& s, const type& v)
  {
    s.obj["From"] = pathToString(v.from);
    s.obj["To"] = pathToString(v.to);
    s.obj["Source"] = v.source;
    s.obj["Target"] = v.target;
    s.obj["Riddle"] = toJsonObject(v.riddle);
  }

  static void writeTo(JSONObject::Deserializer& s, type& v)
  {
    v.from = SEGMent::pathFromString<SEGMent::SceneModel>(
        s.obj["From"].toString());
    v.to
        = SEGMent::pathFromString<SEGMent::SceneModel>(s.obj["To"].toString());
    v.source = s.obj["Source"].toInt();
    v.target = s.obj["Target"].toInt();
    v.riddle = fromJsonObject<SEGMent::riddle_t>(s.obj["Riddle"].toObject());
  }
};

Q_DECLARE_METATYPE(SEGMent::transition_t)
Q_DECLARE_METATYPE(SEGMent::TransitionModel::FadeMode)

W_REGISTER_ARGTYPE(SEGMent::transition_t)
W_REGISTER_ARGTYPE(SEGMent::TransitionModel::FadeMode)

DEFAULT_MODEL_METADATA(SEGMent::TransitionModel, "Transition")
