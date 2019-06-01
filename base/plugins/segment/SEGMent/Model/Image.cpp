#include <score/serialization/DataStreamVisitor.hpp>
#include <score/serialization/JSONVisitor.hpp>

#include <SEGMent/Model/Image.hpp>

namespace SEGMent
{
}

template <>
void DataStreamReader::read(const SEGMent::Image& v)
{
  m_stream << v.path;

  insertDelimiter();
}

template <>
void DataStreamWriter::write(SEGMent::Image& v)
{
  m_stream >> v.path;

  checkDelimiter();
}

template <>
void JSONObjectReader::read(const SEGMent::Image& v)
{
  obj["Image"] = v.path;
}

template <>
void JSONObjectWriter::write(SEGMent::Image& v)
{
  v.path = obj["Image"].toString();
}
