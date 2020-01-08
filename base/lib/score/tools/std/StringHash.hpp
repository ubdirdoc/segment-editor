#pragma once
#include <QHashFunctions>
#include <QString>

#include <functional>


#if (QT_VERSION < QT_VERSION_CHECK(5, 14, 0))
    // TODO merge String.hpp here
namespace std
{
template <>
struct hash<QString>
{
  std::size_t operator()(const QString& path) const { return qHash(path); }
};
} // namespace std
#endif