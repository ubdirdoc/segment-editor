#pragma once
#include <score/model/Entity.hpp>
#include <score/selection/Selectable.hpp>
#include <score/tools/std/HashMap.hpp>

namespace std
{
template <>
struct hash<QString>
{
  std::size_t operator()(const QString& p) const noexcept
  {
    return qHash(p);
  }
};
}
namespace SEGMent
{
//! Transitions can be :
//! - Between scene and scene
//!   - Such a transition can have an associated riddle
//!     - Riddles can be either through input text or gif
//!     - There is a set of associations between inputs to the riddle and texts
//!     that are shown to the player
//!       - The correct answer
//!       - The default bad answer
//!       - Various levels of answers in-between
//! - Between object and scene
//! - Between click area and scene


//! The player must set the gifs at the correct frame.
struct GifRiddle
{
  friend bool operator==(const GifRiddle& lhs, const GifRiddle& rhs) noexcept
  { return true; }
  friend bool operator!=(const GifRiddle& lhs, const GifRiddle& rhs) noexcept
  { return false; }
};

//! The player must complete the puzzle.
struct PuzzleRiddle
{
  friend bool operator==(const PuzzleRiddle& lhs, const PuzzleRiddle& rhs) noexcept
  { return true; }
  friend bool operator!=(const PuzzleRiddle& lhs, const PuzzleRiddle& rhs) noexcept
  { return false; }
};

//! The player must input the correct text.
struct TextRiddle
{
  QString question;
  QString expected;
  QString ifCorrect;
  QString ifWrong;
  QStringList fuzzyMatches;
  bool useStars{};

  friend bool operator==(const TextRiddle& lhs, const TextRiddle& rhs) noexcept
  {
    return lhs.question == rhs.question &&
        lhs.expected == rhs.expected &&
        lhs.ifCorrect == rhs.ifCorrect &&
        lhs.ifWrong == rhs.ifWrong &&
        lhs.fuzzyMatches == rhs.fuzzyMatches &&
        lhs.useStars == rhs.useStars;
  }
  friend bool operator!=(const TextRiddle& lhs, const TextRiddle& rhs) noexcept
  { return !(lhs == rhs); }
};

using riddle_t = eggs::variant<GifRiddle, PuzzleRiddle, TextRiddle>;

}

JSON_METADATA(SEGMent::GifRiddle, "Gif")
JSON_METADATA(SEGMent::PuzzleRiddle, "Puzzle")
JSON_METADATA(SEGMent::TextRiddle, "Text")
