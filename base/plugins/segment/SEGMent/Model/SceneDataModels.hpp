#pragma once
#include <QString>
#include <verdigris>

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

  //! Note: with C++20 all of these can be simplified to
  //! friend bool operator<=>(const LongText& lhs, const LongText& rhs) = default;

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
}

Q_DECLARE_METATYPE(SEGMent::LongText)
W_REGISTER_ARGTYPE(SEGMent::LongText)


namespace SEGMent
{
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

Q_DECLARE_METATYPE(SEGMent::JournalEntry)
W_REGISTER_ARGTYPE(SEGMent::JournalEntry)
