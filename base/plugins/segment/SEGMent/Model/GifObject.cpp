#include "GifObject.hpp"

#include <QtGlobal>

#include <SEGMent/FilePath.hpp>
#include <wobjectimpl.h>
W_OBJECT_IMPL(SEGMent::GifModel)
namespace SEGMent
{

GifModel::GifModel(const Id<GifModel>& id, QObject* parent)
    : base_type{id, "GifObject", parent}
{
  gif.setCacheMode(QMovie::CacheAll);
}

const Image& GifModel::image() const MSVC_NOEXCEPT
{
  return m_image;
}

void GifModel::setImage(const Image& v) MSVC_NOEXCEPT
{
  if (m_image != v)
  {
    m_image = v;
    gif.setFileName(
        toLocalFile(m_image, score::IDocument::documentContext(*parent())));
    m_frames.resize(gif.frameCount());
    setDefaultFrame(m_defaultFrame); // used to clamp if necessary

    imageChanged(v);
  }
}

int GifModel::defaultFrame() const MSVC_NOEXCEPT
{
  return m_defaultFrame;
}

void GifModel::setDefaultFrame(int v) MSVC_NOEXCEPT
{
  v = qBound(0, v, (int)m_frames.size() - 1);
  if (m_defaultFrame != v)
  {
    m_defaultFrame = v;

    defaultFrameChanged(v);
  }
}

const GifModel::Frames& GifModel::frames() const MSVC_NOEXCEPT
{
  return m_frames;
}

void GifModel::setFrames(const Frames& v) MSVC_NOEXCEPT
{
  if (m_frames != v)
  {
    m_frames = v;
    framesChanged(v);
  }
}
} // namespace SEGMent

template <>
void DataStreamReader::read(const SEGMent::GifModel& v)
{
  m_stream << v.m_image << v.m_frames << v.m_defaultFrame;

  insertDelimiter();
}

template <>
void DataStreamWriter::write(SEGMent::GifModel& v)
{
  m_stream >> v.m_image >> v.m_frames >> v.m_defaultFrame;

  v.gif.setCacheMode(QMovie::CacheAll);
  v.gif.setFileName(
      toLocalFile(v.m_image, score::IDocument::documentContext(*v.parent())));
  v.m_frames.resize(v.gif.frameCount());
  v.setDefaultFrame(v.m_defaultFrame); // used to clamp if necessary
  checkDelimiter();
}

template <>
void JSONObjectReader::read(const SEGMent::GifModel& v)
{
  obj["Image"] = v.m_image.path;
  obj["Frames"] = toJsonValueArray(v.m_frames);
  obj["Default"] = v.m_defaultFrame;
}

template <>
void JSONObjectWriter::write(SEGMent::GifModel& v)
{
  v.m_image.path = obj["Image"].toString();
  v.gif.setCacheMode(QMovie::CacheAll);
  v.gif.setFileName(
      toLocalFile(v.m_image, score::IDocument::documentContext(*v.parent())));
  v.m_frames
      = fromJsonValueArray<SEGMent::GifModel::Frames>(obj["Frames"].toArray());
  v.m_defaultFrame = obj["Default"].toInt();
}
