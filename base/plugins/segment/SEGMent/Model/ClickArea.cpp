#include "ClickArea.hpp"

#include <wobjectimpl.h>
W_OBJECT_IMPL(SEGMent::ClickAreaModel)
namespace SEGMent
{

ClickAreaModel::ClickAreaModel(Id<ClickAreaModel> id, QObject* parent)
    : base_type{std::move(id), "ClickArea", parent}
{
}
} // namespace SEGMent

template <>
void DataStreamReader::read(const SEGMent::ClickAreaModel& v)
{
  insertDelimiter();
}

template <>
void DataStreamWriter::write(SEGMent::ClickAreaModel& v)
{
  checkDelimiter();
}

template <>
void JSONObjectReader::read(const SEGMent::ClickAreaModel& v)
{
}

template <>
void JSONObjectWriter::write(SEGMent::ClickAreaModel& v)
{
}
