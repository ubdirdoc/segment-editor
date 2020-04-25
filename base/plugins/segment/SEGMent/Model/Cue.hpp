#pragma once
#include <QStringList>
#include <QVector>
namespace SEGMent
{
struct Cue {
  QString key;
  QStringList cues;

  friend bool operator==(const Cue& lhs, const Cue& rhs) noexcept
  {
      return lhs.key == rhs.key && lhs.cues == rhs.cues;
  }
  friend bool operator!=(const Cue& lhs, const Cue& rhs) noexcept
  {
      return !(lhs == rhs);
  }
};
using Cues = QVector<Cue>;
}
