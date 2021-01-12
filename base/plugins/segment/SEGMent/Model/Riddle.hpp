#pragma once
#include <score/model/Entity.hpp>
#include <score/selection/Selectable.hpp>
#include <score/tools/std/HashMap.hpp>
#include <score/tools/std/StringHash.hpp>
#include <score/serialization/VariantSerialization.hpp>
/**
 * This file contains the data model for the various riddles.
 */
namespace SEGMent
{
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
using riddle_impl_type = eggs::variant<GifRiddle, PuzzleRiddle, TextRiddle>;
struct riddle_t : riddle_impl_type
{
  riddle_t(riddle_impl_type t): riddle_impl_type{std::move(t)} { }

  riddle_t() = default;
  riddle_t(const riddle_t&) = default;
  riddle_t(riddle_t&&) = default;
  riddle_t& operator=(const riddle_t&) = default;
  riddle_t& operator=(riddle_t&&) = default;

  //! Riddles can have a time limit, expressed in seconds.
  optional<int> maxTime{};

  friend bool operator==(const riddle_t& lhs, const riddle_t& rhs) noexcept
  {
    return static_cast<const riddle_impl_type&>(lhs) == static_cast<const riddle_impl_type&>(rhs) &&
        lhs.maxTime == rhs.maxTime;
  }
  friend bool operator!=(const riddle_t& lhs, const riddle_t& rhs) noexcept
  {
    return !(lhs == rhs);
  }
};



} // namespace SEGMent

// Serialization code.

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
    s.readFrom(static_cast<const SEGMent::riddle_impl_type&>(v));
    s.stream() << v.maxTime;
  }

  static void writeTo(DataStream::Deserializer& s, SEGMent::riddle_t& v)
  {
    auto& variant = static_cast<SEGMent::riddle_impl_type&>(v);
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
    s.readFrom(static_cast<const SEGMent::riddle_impl_type&>(v));
    s.obj["MaxTime"] = toJsonValue(v.maxTime);
  }

  static void writeTo(JSONObject::Deserializer& s, type& v)
  {
    auto& variant = static_cast<SEGMent::riddle_impl_type&>(v);
    s.writeTo(variant);
    v.maxTime = fromJsonValue<optional<int>>(s.obj["MaxTime"]);
  }
};

JSON_METADATA(SEGMent::GifRiddle, "Gif")
JSON_METADATA(SEGMent::PuzzleRiddle, "Puzzle")
JSON_METADATA(SEGMent::TextRiddle, "Text")
