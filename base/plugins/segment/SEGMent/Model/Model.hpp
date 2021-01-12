#pragma once
#include <score/model/Entity.hpp>
#include <score/selection/Selectable.hpp>
#include <SEGMent/StringUtils.hpp>

namespace SEGMent
{
//! Used to add the Path field to the json for every serializable object.
//! This is only used by the engine, not by the editor.
template <typename T>
class PathAsId : public T
{
  friend struct TSerializer<JSONObject, SEGMent::PathAsId<T>>;
  friend struct TSerializer<DataStream, SEGMent::PathAsId<T>>;

public:
  using base_type = T;
  using T::T;

  template <typename DeserializerVisitor>
  PathAsId(DeserializerVisitor&& vis, QObject* parent)
      : T{std::forward<DeserializerVisitor>(vis), parent}
  {
    vis.writeTo(*this);
  }
};

} // namespace SEGMent

template <typename T>
struct TSerializer<DataStream, SEGMent::PathAsId<T>>
{
  static void
  readFrom(DataStream::Serializer& s, const SEGMent::PathAsId<T>& v)
  {
  }

  static void writeTo(DataStream::Deserializer& s, SEGMent::PathAsId<T>& v) {}
};

template <typename T>
struct TSerializer<JSONObject, SEGMent::PathAsId<T>>
{
  static void
  readFrom(JSONObject::Serializer& s, const SEGMent::PathAsId<T>& v)
  {
    s.obj["Path"] = pathToString(score::IDocument::path(v));
  }

  static void writeTo(JSONObject::Deserializer& s, SEGMent::PathAsId<T>& v) {}
};

template <typename T>
struct is_custom_serialized<SEGMent::PathAsId<T>> : std::true_type
{
};
