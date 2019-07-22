#pragma once
#include <QPixmap>
#include <QString>

#include <verdigris>

namespace SEGMent
{

//! Simple wrapper for a path to an image
struct Image
{
  explicit Image(QString p) : path{std::move(p)} {}

  Image() {}
  Image(const Image&) = default;
  Image(Image&&) = default;
  Image& operator=(const Image&) = default;
  Image& operator=(Image&&) = default;

  QString path;

  operator QString&() { return path; }
  operator const QString&() const { return path; }
  bool operator==(const Image& other) { return other.path == path; }
  bool operator!=(const Image& other) { return other.path != path; }
};

} // namespace SEGMent

W_REGISTER_ARGTYPE(SEGMent::Image)
Q_DECLARE_METATYPE(SEGMent::Image)
