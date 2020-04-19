#include "Transition.hpp"

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
} // namespace SEGMent

template <>
void DataStreamReader::read(const SEGMent::TransitionModel& v)
{
  m_stream << v.m_transition << v.m_fade << v.m_color << v.m_sound << v.m_unique
           << v.m_eventToAdd
           << v.m_eventToRemove
           << v.m_eventRequired
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

  auto processEventString = [] (const QString& str) {
      QJsonArray array;

      for(auto& elt : str.split(";")) {
          if(auto str = elt.trimmed(); !str.isEmpty())
              array.push_back(str);
      }

      return array;
  };

  obj["EventsToAdd"]    = processEventString(v.m_eventToAdd);
  obj["EventsToRemove"] = processEventString(v.m_eventToRemove);
  obj["EventsRequired"] = processEventString(v.m_eventRequired);
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

  auto processEventString = [] (const QJsonArray& array) {
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

  v.m_eventToAdd    = processEventString(obj["EventsToAdd"].toArray());
  v.m_eventToRemove = processEventString(obj["EventsToRemove"].toArray());
  v.m_eventRequired = processEventString(obj["EventsRequired"].toArray());
}
