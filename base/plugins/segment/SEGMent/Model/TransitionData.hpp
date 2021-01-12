#pragma once

#include <score/model/path/Path.hpp>
#include <SEGMent/Items/Anchor.hpp>
#include <SEGMent/Model/Riddle.hpp>

namespace SEGMent
{
/**
 * Base data model for transitions.
 * A transition goes from an object to another,
 * with given visual anchors.
 *
 * Transitions for most objects are generic.
 *
 * Transitions can be :
 * - Between scene and scene
 *   - Such a transition can have an associated riddle
 *     - Riddles can be either through input text or gif
 *     - There is a set of associations between inputs to the riddle and texts
 *     that are shown to the player
 *       - The correct answer
 *       - The default bad answer
 *       - Various levels of answers in-between
 * - Between object and scene
 * - Between click area and scene
 */
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

/**
 * We have the special case of transitions from scenes to scenes,
 * which have additionally an optional riddle (puzzle, etc).
 */
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

// Here we define the potential types of transitions
using SceneToScene = Transition<SceneModel, SceneModel>;
using ObjectToScene = Transition<ImageModel, SceneModel>;
using GifToScene = Transition<GifModel, SceneModel>;
using ClickToScene = Transition<ClickAreaModel, SceneModel>;

using transition_t
    = eggs::variant<SceneToScene, ObjectToScene, GifToScene, ClickToScene>;
}


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
W_REGISTER_ARGTYPE(SEGMent::transition_t)
