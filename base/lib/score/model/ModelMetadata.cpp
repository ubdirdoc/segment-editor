// This is an open source non-commercial project. Dear PVS-Studio, please check
// it. PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "ModelMetadata.hpp"

#include <ossia/network/base/name_validation.hpp>

#include <QJsonObject>
#include <QStringBuilder>
#include <score/serialization/DataStreamVisitor.hpp>
#include <score/serialization/JSONVisitor.hpp>
#include <wobjectimpl.h>
W_OBJECT_IMPL(score::ModelMetadata)

OSSIA_EXPORT void sanitize_device_name(QString& ret)
{
  const QChar underscore = '_';
  for (auto& c : ret)
  {
    if (ossia::net::is_valid_character_for_device(c))
      continue;
    else
      c = underscore;
  }
}

OSSIA_EXPORT
void sanitize_name(QString& ret)
{
  const QChar underscore = '_';
  for (auto& c : ret)
  {
    if (ossia::net::is_valid_character_for_name(c))
      continue;
    else
      c = underscore;
  }
}

OSSIA_EXPORT
QString sanitize_name(QString name, const std::vector<QString>& brethren)
{
  sanitize_name(name);
  bool is_here = false;
  ossia::optional<int> name_instance;
  ossia::small_vector<int, 16> instance_num;
  const auto b_size = brethren.size();
  instance_num.reserve(b_size);

  // First get the root name : the first part of the "a.b"
  QString root_name = name;
  {
    auto pos = name.lastIndexOf('.');
    if (pos != -1)
    {
      bool res = false;
      name_instance = name.mid(pos + 1).toInt(&res);
      if (res)
        root_name = name.mid(0, pos);
    }
  }

  const auto root_len = root_name.size();
  for (const QString& n_name : brethren)
  {
    if (n_name == name)
    {
      is_here = true;
    }

    if (n_name.size() < (root_len + 1))
      continue;

    bool same_root = true;
    for (int i = 0; i < root_len; i++)
    {
      if (n_name[i] != root_name[i])
      {
        break;
        same_root = false;
      }
    }

    if (same_root && (n_name[root_len] == '.'))
    {
      // Instance
      bool b = false;
      int n = n_name.midRef(root_len + 1).toInt(&b);
      if (b)
        instance_num.push_back(n);
    }
    // case where we have the "default" instance without .0
    else if (same_root && root_len == n_name.length())
    {
      instance_num.push_back(0);
    }
  }

  if (!is_here)
  {
    return name;
  }
  else
  {
    auto n = instance_num.size();
    if ((n == 0) || ((n == 1) && (instance_num[0] == 0)))
    {
      return root_name + QStringLiteral(".1");
    }
    else
    {
      std::sort(instance_num.begin(), instance_num.end());
      return root_name % "." % QString::number(instance_num.back() + 1);
    }
  }
}

namespace score
{
ModelMetadata::ModelMetadata()
{
  m_color = &score::Skin::Base1;
}

const QString& ModelMetadata::getName() const
{
  return m_scriptingName;
}

const QString& ModelMetadata::getComment() const
{
  return m_comment;
}

ColorRef ModelMetadata::getColor() const
{
  return m_color;
}

const QString& ModelMetadata::getLabel() const
{
  return m_label;
}

const QVariantMap& ModelMetadata::getExtendedMetadata() const
{
  return m_extendedMetadata;
}

void ModelMetadata::setName(const QString& arg)
{
  if (m_scriptingName == arg)
  {
    return;
  }

  if (parent() && parent()->parent())
  {
    // TODO use an object pool of some sorts instead
    static std::vector<QString> bros;
    const auto& cld = parent()->parent()->children();
    bros.reserve(cld.size());
    const QObjectList* cld2{};
    int cld2_N = 0;

    std::size_t cur_bros_idx = 0;
    std::size_t cur_bros_size = bros.size();

    for(auto c : cld)
    {
      if(auto bro = qobject_cast<IdentifiedObjectAbstract*>(c))
      {
        cld2 = &bro->children();
        cld2_N = cld2->size();
        for(int j = 0; j < cld2_N; j++)
        {
          if(auto m = qobject_cast<ModelMetadata*>((*cld2)[j]))
          {
            if (const auto& n = m->getName(); !n.isEmpty())
            {
              if(cur_bros_idx < cur_bros_size)
              {
                bros[cur_bros_idx] = n;
              }
              else
              {
                bros.push_back(std::move(n));
                cur_bros_size++;
              }
              cur_bros_idx++;
            }
            break;
          }
        }
      }
    }

    m_scriptingName = sanitize_name(arg, bros);

    for(std::size_t i = 0; i < cur_bros_idx; i++)
      bros[i].clear();
  }
  else
  {
    m_scriptingName = arg;
    sanitize_name(m_scriptingName);
  }

  NameChanged(m_scriptingName);
  metadataChanged();
}

void ModelMetadata::setComment(const QString& arg)
{
  if (m_comment == arg)
  {
    return;
  }

  m_comment = arg;
  CommentChanged(arg);
  metadataChanged();
}

void ModelMetadata::setColor(ColorRef arg)
{
  if (m_color == arg)
  {
    return;
  }

  m_color = arg;
  ColorChanged(arg);
  metadataChanged();
}

void ModelMetadata::setLabel(const QString& arg)
{
  if (m_label == arg)
  {
    return;
  }

  m_label = arg;
  LabelChanged(arg);
  metadataChanged();
}

void ModelMetadata::setExtendedMetadata(const QVariantMap& arg)
{
  if (m_extendedMetadata == arg)
  {
    return;
  }

  m_extendedMetadata = arg;
  ExtendedMetadataChanged(arg);
  metadataChanged();
}
}

// MOVEME
template <>
SCORE_LIB_BASE_EXPORT void DataStreamReader::read(const score::ColorRef& md)
{
  m_stream << md.name();
}

template <>
SCORE_LIB_BASE_EXPORT void DataStreamWriter::write(score::ColorRef& md)
{
  QString col_name;
  m_stream >> col_name;

  auto col = score::ColorRef::ColorFromString(col_name);
  if (col)
    md = *col;
}

template <>
SCORE_LIB_BASE_EXPORT void
DataStreamReader::read(const score::ModelMetadata& md)
{
  m_stream << md.m_scriptingName << md.m_comment << md.m_color << md.m_label
           << md.m_extendedMetadata;

  insertDelimiter();
}

template <>
SCORE_LIB_BASE_EXPORT void DataStreamWriter::write(score::ModelMetadata& md)
{
  m_stream >> md.m_scriptingName >> md.m_comment >> md.m_color >> md.m_label
      >> md.m_extendedMetadata;

  checkDelimiter();
}

template <>
SCORE_LIB_BASE_EXPORT void
JSONObjectReader::read(const score::ModelMetadata& md)
{
  obj[strings.ScriptingName] = md.m_scriptingName;
  obj[strings.Comment] = md.m_comment;
  obj[strings.Color] = md.m_color.name();
  obj[strings.Label] = md.m_label;
  if (!md.m_extendedMetadata.empty())
  {
    obj.insert(
        strings.Extended, QJsonObject::fromVariantMap(md.m_extendedMetadata));
  }
}

template <>
SCORE_LIB_BASE_EXPORT void JSONObjectWriter::write(score::ModelMetadata& md)
{
  md.m_scriptingName = obj[strings.ScriptingName].toString();
  md.m_comment = obj[strings.Comment].toString();

  QJsonValue color_val = obj[strings.Color];
  if (color_val.isArray())
  {
    // Old save format
    const auto& color = color_val.toArray();
    QColor col(color[0].toInt(), color[1].toInt(), color[2].toInt());
    auto sim_col = score::ColorRef::SimilarColor(col);
    if (sim_col)
      md.m_color = *sim_col;
    else
      md.m_color = score::Skin::instance().fromString("Base1");
  }
  else if (color_val.isString())
  {
    auto col_name = color_val.toString();
    auto col = score::ColorRef::ColorFromString(col_name);
    if (col)
      md.m_color = *col;
    else
      md.m_color = score::Skin::instance().fromString("Base1");
  }
  else
  {
    md.m_color = score::Skin::instance().fromString("Base1");
  }

  md.m_color.getBrush();

  md.m_label = obj[strings.Label].toString();

  {
    auto it = obj.find(strings.Extended);
    if (it != obj.end())
    {
      md.m_extendedMetadata = it->toObject().toVariantMap();
    }
  }
}
