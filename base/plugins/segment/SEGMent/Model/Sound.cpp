#include "Sound.hpp"

#include <wobjectimpl.h>
W_OBJECT_IMPL(SEGMent::Sound)
namespace SEGMent
{

Sound::Sound(const Sound& other)
  : QObject{}
  , m_path{other.m_path}
  , m_volume{other.m_volume}
  , m_range{other.m_range}
  , m_repeat{other.m_repeat}
{
}

Sound::Sound(Sound&& other)
  : QObject{}
  , m_path{other.m_path}
  , m_volume{other.m_volume}
  , m_range{other.m_range}
  , m_repeat{other.m_repeat}
{
}

Sound& Sound::operator=(const Sound& other)
{
  m_path = other.m_path;
  m_volume = other.m_volume;
  m_range = other.m_range;
  m_repeat = other.m_repeat;
  return *this;
}

Sound& Sound::operator=(Sound&& other)
{
  m_path = other.m_path;
  m_volume = other.m_volume;
  m_range = other.m_range;
  m_repeat = other.m_repeat;
  return *this;
}

bool operator==(const Sound& lhs, const Sound& rhs)
{
  return lhs.m_path == rhs.m_path
      && lhs.m_volume == rhs.m_volume
      && lhs.m_range == rhs.m_range
      && lhs.m_repeat == rhs.m_repeat
      ;
}

bool operator!=(const Sound& lhs, const Sound& rhs)
{
  return !(lhs == rhs);
}

const QString& Sound::path() const MSVC_NOEXCEPT
{
  return m_path;
}

void Sound::setPath(const QString& v) MSVC_NOEXCEPT
{
  if (m_path != v)
  {
    m_path = v;
    pathChanged(v);
  }
}

const double& Sound::volume() const MSVC_NOEXCEPT
{
  return m_volume;
}

void Sound::setVolume(const double& v) MSVC_NOEXCEPT
{
  if (m_volume != v)
  {
    m_volume = v;
    volumeChanged(v);
  }
}

const double& Sound::range() const MSVC_NOEXCEPT
{
  return m_range;
}

void Sound::setRange(const double& v) MSVC_NOEXCEPT
{
  if (m_range != v)
  {
    m_range = v;
    rangeChanged(v);
  }
}


bool Sound::repeat() const MSVC_NOEXCEPT
{
  return m_repeat;
}

void Sound::setRepeat(bool v) MSVC_NOEXCEPT
{
  if (m_repeat != v)
  {
    m_repeat = v;
    repeatChanged(v);
  }
}

}

template <>
void DataStreamReader::read(const SEGMent::Sound& v)
{
  m_stream << v.m_path << v.m_volume << v.m_range << v.m_repeat;

  insertDelimiter();
}

template <>
void DataStreamWriter::write(SEGMent::Sound& v)
{
  m_stream >> v.m_path >> v.m_volume >> v.m_range >> v.m_repeat;

  checkDelimiter();
}

template <>
void JSONObjectReader::read(const SEGMent::Sound& v)
{
  obj["Path"] = v.m_path;
  obj["Volume"] = v.m_volume;
  obj["Range"] = v.m_range;
  obj["Repeat"] = v.m_repeat;
}

template <>
void JSONObjectWriter::write(SEGMent::Sound& v)
{
  v.m_path = obj["Path"].toString();
  v.m_volume = obj["Volume"].toDouble();
  v.m_range = obj["Range"].toDouble();
  if(auto it = obj.find("Repeat"); it != obj.end())
      v.m_repeat = it->toBool();
  else
      v.m_repeat = true;
}
