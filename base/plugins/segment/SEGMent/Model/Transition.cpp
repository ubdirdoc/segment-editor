#include "Transition.hpp"
#include <SEGMent/StringUtils.hpp>

#include <score/serialization/VariantSerialization.hpp>

#include <wobjectimpl.h>
W_OBJECT_IMPL(SEGMent::TransitionModel)
namespace SEGMent
{

TransitionModel::TransitionModel(
    transition_t trans,
    const Id<TransitionModel>& id,
    QObject* parent)
    : Entity{id, "Transition", parent}, m_transition{std::move(trans)}
{
}

const transition_t& TransitionModel::transition() const MSVC_NOEXCEPT
{
  return m_transition;
}

void TransitionModel::setTransition(const transition_t& v) MSVC_NOEXCEPT
{
  if (m_transition != v)
  {
    m_transition = v;
    transitionChanged(v);
  }
}

const TransitionModel::FadeMode& TransitionModel::fade() const MSVC_NOEXCEPT
{
  return m_fade;
}

void TransitionModel::setFade(const TransitionModel::FadeMode& v) MSVC_NOEXCEPT
{
  if (m_fade != v)
  {
    m_fade = v;
    fadeChanged(v);
  }
}

QColor TransitionModel::color() const MSVC_NOEXCEPT
{
  return m_color;
}

void TransitionModel::setColor(QColor v) MSVC_NOEXCEPT
{
  if (m_color != v)
  {
    m_color = v;
    colorChanged(v);
  }
}

const Sound& TransitionModel::sound() const MSVC_NOEXCEPT
{
  return m_sound;
}
Sound& TransitionModel::sound_ref() MSVC_NOEXCEPT
{
  return m_sound;
}

void TransitionModel::setSound(const Sound& v) MSVC_NOEXCEPT
{
  if (m_sound != v)
  {
    m_sound = v;
    soundChanged(v);
  }
}

bool TransitionModel::unique() const MSVC_NOEXCEPT
{
  return m_unique;
}

void TransitionModel::setUnique(bool v) MSVC_NOEXCEPT
{
  if (m_unique != v)
  {
    m_unique = v;
    uniqueChanged(v);
  }
}

const QString& TransitionModel::eventToAdd() const MSVC_NOEXCEPT
{
  return m_eventToAdd;
}

void TransitionModel::setEventToAdd(const QString& v) MSVC_NOEXCEPT
{
  if (m_eventToAdd != v)
  {
    m_eventToAdd = v;
    eventToAddChanged(v);
  }
}
const QString& TransitionModel::eventToRemove() const MSVC_NOEXCEPT
{
  return m_eventToRemove;
}

void TransitionModel::setEventToRemove(const QString& v) MSVC_NOEXCEPT
{
  if (m_eventToRemove != v)
  {
    m_eventToRemove = v;
    eventToRemoveChanged(v);
  }
}

const QString& TransitionModel::eventRequired() const MSVC_NOEXCEPT
{
  return m_eventRequired;
}

void TransitionModel::setEventRequired(const QString& v) MSVC_NOEXCEPT
{
  if (m_eventRequired != v)
  {
    m_eventRequired = v;
    eventRequiredChanged(v);
  }
}

const QString& TransitionModel::video() const MSVC_NOEXCEPT
{
  return m_video;
}

void TransitionModel::setVideo(const QString& v) MSVC_NOEXCEPT
{
  if (m_video != v)
  {
    m_video = v;
    videoChanged(v);
  }
}
bool TransitionModel::videoEachTime() const MSVC_NOEXCEPT
{
  return m_videoEachTime;
}

void TransitionModel::setVideoEachTime(bool v) MSVC_NOEXCEPT
{
  if (m_videoEachTime != v)
  {
    m_videoEachTime = v;
    videoEachTimeChanged(v);
  }
}

} // namespace SEGMent

template <>
void DataStreamReader::read(const SEGMent::TransitionModel& v)
{
  m_stream << v.m_transition << v.m_fade << v.m_color << v.m_sound << v.m_unique
           << v.m_eventToAdd
           << v.m_eventToRemove
           << v.m_eventRequired
           << v.m_video
           << v.m_videoEachTime
  ;

  insertDelimiter();
}

template <>
void DataStreamWriter::write(SEGMent::TransitionModel& v)
{
  m_stream >> v.m_transition >> v.m_fade >> v.m_color >> v.m_sound >> v.m_unique
           >> v.m_eventToAdd
           >> v.m_eventToRemove
           >> v.m_eventRequired
           >> v.m_video
           >> v.m_videoEachTime
          ;

  checkDelimiter();
}

template <>
void JSONObjectReader::read(const SEGMent::TransitionModel& v)
{
  obj["Transition"] = toJsonObject(v.m_transition);
  obj["Fade"] = (int)v.m_fade;
  obj["Color"] = toJsonValue(v.m_color);
  obj["Sound"] = toJsonObject(v.m_sound);
  obj["Unique"] = v.m_unique;

  using namespace SEGMent;
  obj["EventsToAdd"]    = semicolonStringToStringList(v.m_eventToAdd);
  obj["EventsToRemove"] = semicolonStringToStringList(v.m_eventToRemove);
  obj["EventsRequired"] = semicolonStringToStringList(v.m_eventRequired);
  obj["Video"] = v.m_video;
  obj["VideoEachTime"] = v.m_videoEachTime;
}

template <>
void JSONObjectWriter::write(SEGMent::TransitionModel& v)
{
  v.m_transition
      = fromJsonObject<SEGMent::transition_t>(obj["Transition"].toObject());
  v.m_fade = (SEGMent::TransitionModel::FadeMode)obj["Fade"].toInt();
  v.m_sound = fromJsonObject<SEGMent::Sound>(obj["Sound"]);
  v.m_color = fromJsonValue<QColor>(obj["Color"]);
  v.m_unique = obj["Unique"].toBool();


  using namespace SEGMent;
  v.m_eventToAdd    = stringListToSemicolonString(obj["EventsToAdd"].toArray());
  v.m_eventToRemove = stringListToSemicolonString(obj["EventsToRemove"].toArray());
  v.m_eventRequired = stringListToSemicolonString(obj["EventsRequired"].toArray());
  v.m_video = obj["Video"].toString();
  v.m_videoEachTime = obj["VideoEachTime"].toBool();
}
