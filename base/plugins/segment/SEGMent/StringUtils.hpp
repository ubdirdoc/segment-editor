#pragma once
#include <QString>
#include <QStringList>
#include <QJsonArray>

#include <score/model/path/Path.hpp>

namespace SEGMent
{

/**
 * These functions convert back and forth between
 * a JSON string array, and a semicolon-separated string
 * shown to the user, e.g.
 *
 * [ "foo", "bar", "baz" ] <=> "foo ; bar ; baz"
 */

inline
QJsonArray semicolonStringToStringList (const QString& str) {
    QJsonArray array;

    for(auto& elt : str.split(";")) {
        if(auto str = elt.trimmed(); !str.isEmpty())
            array.push_back(str);
    }

    return array;
};

inline
QString stringListToSemicolonString(const QJsonArray& array) {
    QString s;
    for(const auto& elt : array) {
        if(!elt.toString().isEmpty()) {
            s += elt.toString();
            s += " ; ";
        }
    }
    if(!s.isEmpty())
        s.resize(s.size() - 3);

    return s;
};


/**
 * These functions convert back and forth between
 * a score Path (e.g. a list of objects starting from the root of a document)
 * to a string that can be saved to json.
 */

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

}
