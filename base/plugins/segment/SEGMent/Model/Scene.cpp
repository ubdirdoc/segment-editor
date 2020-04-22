#include "Scene.hpp"

#include <QUrl>

#include <SEGMent/Model/Model.hpp>
#include <SEGMent/ImageCache.hpp>
#include <wobjectimpl.h>
W_OBJECT_IMPL(SEGMent::SceneModel)
namespace SEGMent
{

SceneModel::SceneModel(Id<SceneModel> id, QObject* parent)
    : base_type{std::move(id), "Scene", parent}
{
  while(m_cues.size() < 3) m_cues.push_back({});
}

const SceneModel::SceneType& SceneModel::sceneType() const MSVC_NOEXCEPT
{
  return m_sceneType;
}

void SceneModel::setSceneType(const SceneModel::SceneType& v) MSVC_NOEXCEPT
{
  if (m_sceneType != v)
  {
    m_sceneType = v;
    sceneTypeChanged(v);
  }
}

const Sound& SceneModel::ambience() const MSVC_NOEXCEPT
{
  return m_ambience;
}
Sound& SceneModel::ambience_ref() MSVC_NOEXCEPT
{
  return m_ambience;
}

void SceneModel::setAmbience(const Sound& v) MSVC_NOEXCEPT
{
  if (m_ambience != v)
  {
    m_ambience = v;
    ambienceChanged(v);
  }
}

LongText SceneModel::startText() const MSVC_NOEXCEPT
{
  return {m_startText};
}

void SceneModel::setStartText(const LongText& v) MSVC_NOEXCEPT
{
  if (m_startText != v)
  {
    m_startText = v;
    startTextChanged(v);
  }
}

const Image& SceneModel::image() const MSVC_NOEXCEPT
{
  return m_image;
}

void SceneModel::setImage(const Image& v) MSVC_NOEXCEPT
{
  if (m_image != v)
  {
    if (metadata().getLabel().isEmpty())
      metadata().setLabel(QUrl(v).fileName().split(".")[0]);
    m_image = v;
    imageChanged(v);
  }
}

const QRectF& SceneModel::rect() const MSVC_NOEXCEPT
{
  return m_rect;
}

void SceneModel::setRect(const QRectF& v) MSVC_NOEXCEPT
{
  if (m_rect != v)
  {
    m_rect = v;
    rectChanged(v);
  }
}

bool SceneModel::repeatText() const MSVC_NOEXCEPT
{
  return m_repeatText;
}

void SceneModel::setRepeatText(bool v) MSVC_NOEXCEPT
{
  if (m_repeatText != v)
  {
    m_repeatText = v;
    repeatTextChanged(v);
  }
}

bool SceneModel::sonar() const MSVC_NOEXCEPT
{
  return m_sonar;
}

void SceneModel::setSonar(bool v) MSVC_NOEXCEPT
{
  if (m_sonar != v)
  {
    m_sonar = v;
    sonarChanged(v);
  }
}

const QStringList& SceneModel::cue() const MSVC_NOEXCEPT
{
  return m_cues;
}

void SceneModel::setCue(const QStringList& v) MSVC_NOEXCEPT
{
  if (m_cues != v)
  {
    m_cues = v;
    cueChanged(v);
  }
}

JournalEntry SceneModel::journal() const MSVC_NOEXCEPT
{
  return {m_journal};
}

void SceneModel::setJournal(const JournalEntry& v) MSVC_NOEXCEPT
{
  if (m_journal != v)
  {
    m_journal = v;
    journalChanged(v);
  }
}
} // namespace SEGMent

template <>
void DataStreamReader::read(const SEGMent::LongText& v)
{
  m_stream << v.txt;
}
template <>
void DataStreamWriter::write(SEGMent::LongText& v)
{
  m_stream >> v.txt;
}

template <>
void DataStreamReader::read(const SEGMent::JournalEntry& v)
{
  m_stream << v.txt;
}
template <>
void DataStreamWriter::write(SEGMent::JournalEntry& v)
{
  m_stream >> v.txt;
}

template <>
void DataStreamReader::read(const SEGMent::SceneModel& v)
{
  ossia::for_each_in_tuple(
      std::tie(v.objects, v.gifs, v.clickAreas, v.backClickAreas, v.textAreas),
      [&](const auto& map) {
        m_stream << (int32_t)map.size();
        for (const auto& obj : map)
        {
          readFrom(obj);
        }
      });
  m_stream << v.m_ambience << v.m_image << v.m_rect << v.m_sceneType
           << v.m_startText << v.m_repeatText << v.m_sonar << v.m_cues << v.m_journal;

  insertDelimiter();
}

template <>
void DataStreamWriter::write(SEGMent::SceneModel& v)
{
  ossia::for_each_in_tuple(
      std::tie(v.objects, v.gifs, v.clickAreas, v.backClickAreas, v.textAreas),
      [&](auto& map) {
        using entity_type =
            typename std::remove_reference_t<decltype(map)>::value_type;
        int32_t sz;
        m_stream >> sz;
        for (; sz-- > 0;)
        {
          auto obj = new entity_type{*this, &v};
          map.add(obj);
        }
      });
  m_stream >> v.m_ambience >> v.m_image >> v.m_rect >> v.m_sceneType
      >> v.m_startText >> v.m_repeatText >> v.m_sonar >> v.m_cues >> v.m_journal;

  checkDelimiter();
}

template <>
void JSONObjectReader::read(const SEGMent::SceneModel& v)
{
  obj["Objects"] = toJsonArray(v.objects);
  obj["Gifs"] = toJsonArray(v.gifs);
  obj["ClickAreas"] = toJsonArray(v.clickAreas);
  obj["BackClickAreas"] = toJsonArray(v.backClickAreas);
  obj["TextAreas"] = toJsonArray(v.textAreas);

  obj["Ambience"] = toJsonObject(v.m_ambience);
  obj["Image"] = v.m_image.path;

  auto parent_doc = score::IDocument::documentFromObject(v);
  auto& cache = SEGMent::ImageCache::instance().cache(SEGMent::toLocalFile(v.m_image.path, parent_doc->context()));
  obj["ImageSize"] = QJsonArray{cache.full_size.width(), cache.full_size.height()};
  obj["Rect"] = toJsonValue(v.m_rect);
  obj["SceneType"] = (int)v.m_sceneType;
  obj["StartText"] = v.m_startText;
  obj["RepeatText"] = v.m_repeatText;
  obj["Sonar"] = v.m_sonar;
  obj["Cue"] = toJsonArray(v.m_cues);
  obj["Journal"] = v.m_journal;
}

template <>
void JSONObjectWriter::write(SEGMent::SceneModel& v)
{
  {
    const auto& objs = obj["Objects"].toArray();
    for (const auto& json_vref : objs)
    {
      auto obj = new SEGMent::ImageModel{
          JSONObject::Deserializer{json_vref.toObject()}, &v};
      v.objects.add(obj);
    }
  }
  {
    const auto& objs = obj["Gifs"].toArray();
    for (const auto& json_vref : objs)
    {
      auto obj = new SEGMent::GifModel{
          JSONObject::Deserializer{json_vref.toObject()}, &v};
      v.gifs.add(obj);
    }
  }
  {
    const auto& objs = obj["ClickAreas"].toArray();
    for (const auto& json_vref : objs)
    {
      auto obj = new SEGMent::ClickAreaModel{
          JSONObject::Deserializer{json_vref.toObject()}, &v};
      v.clickAreas.add(obj);
    }
  }
  {
    const auto& objs = obj["BackClickAreas"].toArray();
    for (const auto& json_vref : objs)
    {
      auto obj = new SEGMent::BackClickAreaModel{
          JSONObject::Deserializer{json_vref.toObject()}, &v};
      v.backClickAreas.add(obj);
    }
  }
  {
    const auto& objs = obj["TextAreas"].toArray();
    for (const auto& json_vref : objs)
    {
      auto obj = new SEGMent::TextAreaModel{
          JSONObject::Deserializer{json_vref.toObject()}, &v};
      v.textAreas.add(obj);
    }
  }

  v.m_ambience = fromJsonObject<SEGMent::Sound>(obj["Ambience"]);
  v.m_image.path = obj["Image"].toString();
  v.m_rect = fromJsonValue<QRectF>(obj["Rect"]);
  v.m_sceneType = (SEGMent::SceneModel::SceneType)obj["SceneType"].toInt();
  v.m_startText = obj["StartText"].toString();
  v.m_repeatText = obj["RepeatText"].toBool();
  v.m_sonar = obj["Sonar"].toBool();
  fromJsonArray(obj["Cue"].toArray(), v.m_cues);
  v.m_journal = obj["Journal"].toString();
}
