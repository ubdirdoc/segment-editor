#include "TextArea.hpp"

#include <wobjectimpl.h>
W_OBJECT_IMPL(SEGMent::TextAreaModel)
namespace SEGMent
{

TextAreaModel::TextAreaModel(const Id<TextAreaModel>& id, QObject* parent)
    : base_type{id, "TextArea", parent}
{
}

const QString& TextAreaModel::text() const MSVC_NOEXCEPT
{
  return m_text;
}

void TextAreaModel::setText(const QString& v) MSVC_NOEXCEPT
{
  if (m_text != v)
  {
    m_text = v;
    textChanged(v);
  }
}

TextAreaModel::Behaviour TextAreaModel::behaviour() const MSVC_NOEXCEPT
{
  return m_behaviour;
}

void TextAreaModel::setBehaviour(Behaviour v) MSVC_NOEXCEPT
{
  if (m_behaviour != v)
  {
    m_behaviour = v;
    behaviourChanged(v);
  }
}
} // namespace SEGMent

template <>
void DataStreamReader::read(const SEGMent::TextAreaModel& v)
{
  m_stream << v.m_text << v.m_behaviour;

  insertDelimiter();
}

template <>
void DataStreamWriter::write(SEGMent::TextAreaModel& v)
{
  m_stream >> v.m_text >> v.m_behaviour;

  checkDelimiter();
}

template <>
void JSONObjectReader::read(const SEGMent::TextAreaModel& v)
{
  obj["Text"] = v.m_text;
  obj["Behaviour"] = v.m_behaviour;
}

template <>
void JSONObjectWriter::write(SEGMent::TextAreaModel& v)
{
  v.m_text = obj["Text"].toString();

  v.m_behaviour = (SEGMent::TextAreaModel::Behaviour)obj["Behaviour"].toInt();
}
