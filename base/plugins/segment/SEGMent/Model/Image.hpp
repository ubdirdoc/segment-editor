#pragma once
#include <QPixmap>
#include <QString>

#include <verdigris>

namespace SEGMent
{

/**
 * @brief Simple wrapper for a path to an image.
 *
 * We wrap it in such a type to be able to show a specific widget
 * in the inspector.
 */
struct Image
{
  explicit Image(QString p) : path{std::move(p)} {}

  Image() {}
  Image(const Image&) = default;
  Image(Image&&) = default;
  Image& operator=(const Image&) = default;
  Image& operator=(Image&&) = default;

  QString path;

  operator QString&() noexcept { return path; }
  operator const QString&() const noexcept { return path; }
  bool operator==(const Image& other) const noexcept { return other.path == path; }
  bool operator!=(const Image& other) const noexcept { return other.path != path; }
};

} // namespace SEGMent

W_REGISTER_ARGTYPE(SEGMent::Image)
Q_DECLARE_METATYPE(SEGMent::Image)
