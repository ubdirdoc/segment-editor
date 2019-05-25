#pragma once
#include <score/command/Command.hpp>
namespace SEGMent
{
//! Identifies undo-redo commands of the SEGMent plug-in
inline const CommandGroupKey& CommandFactoryName()
{
  static const CommandGroupKey key{"SEGMent"};
  return key;
}
}
