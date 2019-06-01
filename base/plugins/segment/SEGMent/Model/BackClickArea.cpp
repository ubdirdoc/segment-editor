#include "BackClickArea.hpp"

#include <wobjectimpl.h>
W_OBJECT_IMPL(SEGMent::BackClickAreaModel)
namespace SEGMent
{

BackClickAreaModel::BackClickAreaModel(
    const Id<BackClickAreaModel>& id,
    QObject* parent)
    : base_type{id, "BackClickArea", parent}
{
}

} // namespace SEGMent

template <>
void DataStreamReader::read(const SEGMent::BackClickAreaModel& v)
{
  insertDelimiter();
}

template <>
void DataStreamWriter::write(SEGMent::BackClickAreaModel& v)
{
  checkDelimiter();
}

template <>
void JSONObjectReader::read(const SEGMent::BackClickAreaModel& v)
{
}

template <>
void JSONObjectWriter::write(SEGMent::BackClickAreaModel& v)
{
}
