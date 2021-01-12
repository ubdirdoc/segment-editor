#pragma once
#include <score/serialization/DataStreamVisitor.hpp>
#include <score/serialization/JSONVisitor.hpp>

#include <SEGMent/Model/Model.hpp>
#include <SEGMent/Model/Sound.hpp>
#include <wobjectimpl.h>

/**
 * This file contains various traits that are shared across objects models.
 *
 * A trait contains all the information needed to:
 * - access a given kind of data (getter / setter)
 * - save and load it
 * - notify when the data changes
 * - introspection & reflection data
 *
 */

namespace SEGMent
{
//! A trait for objects that are positioned relatively to their parent
template <typename T>
class RelativePositionable : public T
{
  W_OBJECT(RelativePositionable)
  friend struct TSerializer<JSONObject, SEGMent::RelativePositionable<T>>;
  friend struct TSerializer<DataStream, SEGMent::RelativePositionable<T>>;

  // Pos and size are given relative to parent
public:
  using base_type = T;
  using T::T;

  template <typename DeserializerVisitor>
  RelativePositionable(DeserializerVisitor&& vis, QObject* parent)
      : T{std::forward<DeserializerVisitor>(vis), parent}
  {
    vis.writeTo(*this);
  }

  /// Position ///
  const QPointF& pos() const MSVC_NOEXCEPT { return m_pos; }

  void setPos(const QPointF& v) MSVC_NOEXCEPT
  {
    if (m_pos != v)
    {
      m_pos = v;
      posChanged(v);
    }
  }
  void posChanged(const QPointF& v) W_SIGNAL(posChanged, v);
  PROPERTY(QPointF, pos READ pos WRITE setPos NOTIFY posChanged)
private:
  QPointF m_pos{};

  /// Size ///
public:
  const QSizeF& size() const MSVC_NOEXCEPT { return m_size; }

  void setSize(QSizeF v) MSVC_NOEXCEPT
  {
    if(v.rwidth() <= 0.001) v.rwidth() = 0.001;
    if(v.rheight() <= 0.001) v.rheight() = 0.001;

    if (m_size != v)
    {
      m_size = v;
      sizeChanged(v);
    }
  }

  void sizeChanged(const QSizeF& v) W_SIGNAL(sizeChanged, v);
  PROPERTY(QSizeF, size READ size WRITE setSize NOTIFY sizeChanged)
private:
  QSizeF m_size{};

  /// z-order ///
public:
  int z() const MSVC_NOEXCEPT { return m_z; }

  void setZ(int v) MSVC_NOEXCEPT
  {
    if (m_z != v)
    {
      m_z = v;
      zChanged(v);
    }
  }

  void zChanged(int v) W_SIGNAL(zChanged, v);
  PROPERTY(int, z READ z WRITE setZ NOTIFY zChanged)
private:
  int m_z{};
};

W_OBJECT_IMPL(RelativePositionable<T>, template <typename T>)
} // namespace SEGMent

template <typename T>
struct TSerializer<DataStream, SEGMent::RelativePositionable<T>>
{
  static void readFrom(
      DataStream::Serializer& s,
      const SEGMent::RelativePositionable<T>& v)
  {
    s.stream() << v.m_pos << v.m_size << v.m_z;
  }

  static void
  writeTo(DataStream::Deserializer& s, SEGMent::RelativePositionable<T>& v)
  {
    s.stream() >> v.m_pos >> v.m_size >> v.m_z;
  }
};

template <typename T>
struct TSerializer<JSONObject, SEGMent::RelativePositionable<T>>
{
  static void readFrom(
      JSONObject::Serializer& s,
      const SEGMent::RelativePositionable<T>& v)
  {
    s.obj["Pos"] = toJsonValue(v.m_pos);
    s.obj["Size"] = toJsonValue(v.m_size);
    s.obj["Z"] = v.m_z;
  }

  static void
  writeTo(JSONObject::Deserializer& s, SEGMent::RelativePositionable<T>& v)
  {
    v.m_pos = fromJsonValue<QPointF>(s.obj["Pos"]);
    v.m_size = fromJsonValue<QSizeF>(s.obj["Size"]);
    v.m_z = s.obj["Z"].toInt();
  }
};

template <typename T>
struct is_custom_serialized<SEGMent::RelativePositionable<T>> : std::true_type
{
};

namespace SEGMent
{
//! A trait for objects that have a sound
template <typename T>
class WithSound : public T
{
  W_OBJECT(WithSound)
  friend struct TSerializer<JSONObject, SEGMent::WithSound<T>>;
  friend struct TSerializer<DataStream, SEGMent::WithSound<T>>;

  // Pos and size are given relative to parent
public:
  using base_type = T;
  using T::T;

  template <typename DeserializerVisitor>
  WithSound(DeserializerVisitor&& vis, QObject* parent)
      : T{std::forward<DeserializerVisitor>(vis), parent}
  {
    vis.writeTo(*this);
  }

public:
  const Sound& sound() const MSVC_NOEXCEPT { return m_sound; }
  Sound& sound_ref() MSVC_NOEXCEPT { return m_sound; }
  void setSound(const Sound& v) MSVC_NOEXCEPT
  {
    if (m_sound != v)
    {
      m_sound = v;
      soundChanged(v);
    }
  }
  void soundChanged(const Sound& v) W_SIGNAL(soundChanged, v);
  PROPERTY(Sound, sound READ sound WRITE setSound NOTIFY soundChanged)
private:
  Sound m_sound;
};

W_OBJECT_IMPL(WithSound<T>, template <typename T>)
} // namespace SEGMent

template <typename T>
struct TSerializer<DataStream, SEGMent::WithSound<T>>
{
  static void
  readFrom(DataStream::Serializer& s, const SEGMent::WithSound<T>& v)
  {
    s.stream() << v.m_sound;
  }

  static void writeTo(DataStream::Deserializer& s, SEGMent::WithSound<T>& v)
  {
    s.stream() >> v.m_sound;
  }
};

template <typename T>
struct TSerializer<JSONObject, SEGMent::WithSound<T>>
{
  static void
  readFrom(JSONObject::Serializer& s, const SEGMent::WithSound<T>& v)
  {
    s.obj["Sound"] = toJsonObject(v.m_sound);
  }

  static void writeTo(JSONObject::Deserializer& s, SEGMent::WithSound<T>& v)
  {
    v.m_sound = fromJsonObject<SEGMent::Sound>(s.obj["Sound"]);
  }
};

template <typename T>
struct is_custom_serialized<SEGMent::WithSound<T>> : std::true_type
{
};

namespace SEGMent
{
/**
 * @brief Data model for most objects in a scene:
 *
 * They have:
 * - an identifier
 * - a sound
 * - a position
 */
template <typename T>
using Object = PathAsId<WithSound<RelativePositionable<score::Entity<T>>>>;


/**
 * @brief Data model for objects without an associated sound.
 */
template <typename T>
using SilentObject = PathAsId<RelativePositionable<score::Entity<T>>>;
}
