#pragma once
#include <score/model/Entity.hpp>
#include <score/selection/Selectable.hpp>
#include <score/tools/std/HashMap.hpp>
#include <score/serialization/VariantSerialization.hpp>

namespace std
{
template <>
struct hash<QString>
{
  std::size_t operator()(const QString& p) const noexcept { return qHash(p); }
};
} // namespace std
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
  {
    return true;
  }
  friend bool operator!=(const GifRiddle& lhs, const GifRiddle& rhs) noexcept
  {
    return false;
  }
};

//! The player must complete the puzzle.
struct PuzzleRiddle
{
  friend bool
  operator==(const PuzzleRiddle& lhs, const PuzzleRiddle& rhs) noexcept
  {
    return true;
  }
  friend bool
  operator!=(const PuzzleRiddle& lhs, const PuzzleRiddle& rhs) noexcept
  {
    return false;
  }
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
    return lhs.question == rhs.question && lhs.expected == rhs.expected
           && lhs.ifCorrect == rhs.ifCorrect && lhs.ifWrong == rhs.ifWrong
           && lhs.fuzzyMatches == rhs.fuzzyMatches
           && lhs.useStars == rhs.useStars;
  }
  friend bool operator!=(const TextRiddle& lhs, const TextRiddle& rhs) noexcept
  {
    return !(lhs == rhs);
  }
};

//! A riddle can either be a text, a gif, or a puzzle
struct riddle_t : eggs::variant<GifRiddle, PuzzleRiddle, TextRiddle>
{
  using impl_type = eggs::variant<GifRiddle, PuzzleRiddle, TextRiddle>;
  riddle_t(impl_type t): variant{std::move(t)} { }

  riddle_t() = default;
  riddle_t(const riddle_t&) = default;
  riddle_t(riddle_t&&) = default;
  riddle_t& operator=(const riddle_t&) = default;
  riddle_t& operator=(riddle_t&&) = default;

  //! Riddles can have a time limit, expressed in seconds.
  optional<int> maxTime{};

  friend bool operator==(const riddle_t& lhs, const riddle_t& rhs) noexcept
  {
    return static_cast<const impl_type&>(lhs) == static_cast<const impl_type&>(rhs) &&
        lhs.maxTime == rhs.maxTime;
  }
  friend bool operator!=(const riddle_t& lhs, const riddle_t& rhs) noexcept
  {
    return !(lhs == rhs);
  }
};



} // namespace SEGMent

template <>
struct is_custom_serialized<SEGMent::riddle_t>: std::true_type { };

template <>
struct TSerializer<
    DataStream,
    SEGMent::riddle_t>
{
  using type = SEGMent::riddle_t;
  static void readFrom(DataStream::Serializer& s, const SEGMent::riddle_t& v)
  {
    s.readFrom(static_cast<const typename type::impl_type&>(v));
    s.stream() << v.maxTime;
  }

  static void writeTo(DataStream::Deserializer& s, SEGMent::riddle_t& v)
  {
    auto& variant = static_cast<typename type::impl_type&>(v);
    s.writeTo(variant);
    s.stream() >> v.maxTime;
  }
};

template <>
struct TSerializer<
    JSONObject,
    SEGMent::riddle_t>
{
  using type = SEGMent::riddle_t;
  static void readFrom(JSONObject::Serializer& s, const type& v)
  {
    s.readFrom(static_cast<const typename type::impl_type&>(v));
    s.obj["MaxTime"] = toJsonValue(v.maxTime);
  }

  static void writeTo(JSONObject::Deserializer& s, type& v)
  {
    auto& variant = static_cast<typename type::impl_type&>(v);
    s.writeTo(variant);
    v.maxTime = fromJsonValue<optional<int>>(s.obj["MaxTime"]);
  }
};

JSON_METADATA(SEGMent::GifRiddle, "Gif")
JSON_METADATA(SEGMent::PuzzleRiddle, "Puzzle")
JSON_METADATA(SEGMent::TextRiddle, "Text")
