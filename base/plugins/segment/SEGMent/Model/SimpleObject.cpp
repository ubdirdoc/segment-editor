#include "SimpleObject.hpp"

#include <wobjectimpl.h>
W_OBJECT_IMPL(SEGMent::ImageModel)
namespace SEGMent
{

ImageModel::ImageModel(Id<ImageModel> id, QObject* parent)
    : base_type{std::move(id), "SimpleObject", parent}
{
}

const Image& ImageModel::image() const MSVC_NOEXCEPT
{
  return m_image;
}

void ImageModel::setImage(const Image& v) MSVC_NOEXCEPT
{
  if (m_image != v)
  {
    m_image = v;
    imageChanged(v);
  }
}

bool ImageModel::puzzle() const MSVC_NOEXCEPT
{
  return m_puzzle;
}

void ImageModel::setPuzzle(bool v) MSVC_NOEXCEPT
{
  if (m_puzzle != v)
  {
    m_puzzle = v;
    puzzleChanged(v);
  }
}
} // namespace SEGMent

template <>
void DataStreamReader::read(const SEGMent::ImageModel& v)
{
  m_stream << v.m_image << v.m_puzzle;

  insertDelimiter();
}

template <>
void DataStreamWriter::write(SEGMent::ImageModel& v)
{
  m_stream >> v.m_image >> v.m_puzzle;

  checkDelimiter();
}

template <>
void JSONObjectReader::read(const SEGMent::ImageModel& v)
{
  obj["Image"] = v.m_image.path;
  obj["PuzzlePiece"] = v.m_puzzle;
}

template <>
void JSONObjectWriter::write(SEGMent::ImageModel& v)
{
  v.m_image.path = obj["Image"].toString();
  v.m_puzzle = obj["PuzzlePiece"].toBool();
}
