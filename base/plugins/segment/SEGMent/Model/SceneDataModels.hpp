#pragma once
#include <QString>

namespace SEGMent
{

/**
 * @brief Used to represent a text that should be shown in a big text
 * area in the inspector.
 */
struct LongText
{
  QString txt;
  operator const QString&() const noexcept { return txt; }
  operator QString&() noexcept { return txt; }
  friend bool operator==(const LongText& lhs, const LongText& rhs) noexcept
  {
    return lhs.txt == rhs.txt;
  }
  friend bool operator!=(const LongText& lhs, const LongText& rhs) noexcept
  {
    return lhs.txt != rhs.txt;
  }
  friend bool operator<(const LongText& lhs, const LongText& rhs) noexcept
  {
    return lhs.txt < rhs.txt;
  }
  friend bool operator>(const LongText& lhs, const LongText& rhs) noexcept
  {
    return lhs.txt > rhs.txt;
  }
  friend bool operator<=(const LongText& lhs, const LongText& rhs) noexcept
  {
    return lhs.txt <= rhs.txt;
  }
  friend bool operator>=(const LongText& lhs, const LongText& rhs) noexcept
  {
    return lhs.txt >= rhs.txt;
  }
};

/**
 * @brief Used to represent a text that should be shown as a journal entry
 * in the inspector.
 */
struct JournalEntry
{
  QString txt;
  operator const QString&() const noexcept { return txt; }
  operator QString&() noexcept { return txt; }
  friend bool operator==(const JournalEntry& lhs, const JournalEntry& rhs) noexcept
  {
    return lhs.txt == rhs.txt;
  }
  friend bool operator!=(const JournalEntry& lhs, const JournalEntry& rhs) noexcept
  {
    return lhs.txt != rhs.txt;
  }
  friend bool operator<(const JournalEntry& lhs, const JournalEntry& rhs) noexcept
  {
    return lhs.txt < rhs.txt;
  }
  friend bool operator>(const JournalEntry& lhs, const JournalEntry& rhs) noexcept
  {
    return lhs.txt > rhs.txt;
  }
  friend bool operator<=(const JournalEntry& lhs, const JournalEntry& rhs) noexcept
  {
    return lhs.txt <= rhs.txt;
  }
  friend bool operator>=(const JournalEntry& lhs, const JournalEntry& rhs) noexcept
  {
    return lhs.txt >= rhs.txt;
  }
  JournalEntry& operator=(const QString& other) noexcept { txt = other; return *this; }
};
}
