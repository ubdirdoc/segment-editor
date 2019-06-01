#pragma once
#include <score/model/Entity.hpp>
#include <score/selection/Selectable.hpp>
namespace SEGMent
{

template <typename T>
QString pathToString(const Path<T>& p)
{
  QString s;
  const ObjectPath& path = p.unsafePath();
  for (auto& obj : path.vec())
  {
    s.append("/");
    s.append(obj.objectName());
    s.append(".");
    s.append(QString::number(obj.id()));
  }
  return s;
}

template <typename T>
Path<T> pathFromString(const QString& s)
{
  ObjectPath p;
  const auto l = s.splitRef("/", QString::SplitBehavior::SkipEmptyParts);
  for (auto& frag : l)
  {
    auto dot = frag.lastIndexOf(".");
    QString name = frag.mid(0, dot).toString();
    int number = frag.mid(dot + 1).toInt();
    p.vec().push_back(ObjectIdentifier{name, number});
  }
  return Path<T>{p, typename Path<T>::UnsafeDynamicCreation{}};
}
} // namespace SEGMent

namespace SEGMent
{
//! A trait for objects that have a sound
template <typename T>
class PathAsId : public T
{
  friend struct TSerializer<JSONObject, SEGMent::PathAsId<T>>;
  friend struct TSerializer<DataStream, SEGMent::PathAsId<T>>;

  // Pos and size are given relative to parent
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
