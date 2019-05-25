#pragma once
#include <Process/ProcessMetadata.hpp>

namespace SEGMent
{
class ProcessModel;
}

PROCESS_METADATA(
    ,
    SEGMent::ProcessModel,
    "E9F2455E-3EA9-4753-B322-E0EE26EFE251",
    "SEGMent",
    "SEGMent",
    "SEGMent",
    (QStringList{"SEGMent"}),
    Process::ProcessFlags::SupportsTemporal)
