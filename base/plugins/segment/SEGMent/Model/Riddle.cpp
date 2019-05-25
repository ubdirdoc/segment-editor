#include "Riddle.hpp"

#include <score/serialization/JSONValueVisitor.hpp>
namespace SEGMent
{

}

template <>
void DataStreamReader::read(const SEGMent::GifRiddle& v)
{
  insertDelimiter();
}

template <>
void DataStreamWriter::write(SEGMent::GifRiddle& v)
{
  checkDelimiter();
}

template <>
void JSONObjectReader::read(const SEGMent::GifRiddle& v)
{
}

template <>
void JSONObjectWriter::write(SEGMent::GifRiddle& v)
{
}


template <>
void DataStreamReader::read(const SEGMent::PuzzleRiddle& v)
{
  insertDelimiter();
}

template <>
void DataStreamWriter::write(SEGMent::PuzzleRiddle& v)
{
  checkDelimiter();
}

template <>
void JSONObjectReader::read(const SEGMent::PuzzleRiddle& v)
{
}

template <>
void JSONObjectWriter::write(SEGMent::PuzzleRiddle& v)
{
}

template <>
void DataStreamReader::read(const SEGMent::TextRiddle& v)
{
  m_stream << v.question << v.expected << v.ifCorrect << v.ifWrong << v.fuzzyMatches << v.useStars;

  insertDelimiter();
}

template <>
void DataStreamWriter::write(SEGMent::TextRiddle& v)
{
  m_stream >> v.question >> v.expected >> v.ifCorrect >> v.ifWrong >> v.fuzzyMatches >> v.useStars;

  checkDelimiter();
}

template <>
void JSONObjectReader::read(const SEGMent::TextRiddle& v)
{
  obj["Question"] = v.question;
  obj["Expected"] = v.expected;
  obj["IfCorrect"] = v.ifCorrect;
  obj["IfWrong"] = v.ifWrong;
  obj["FuzzyMatches"] = QJsonArray::fromStringList(v.fuzzyMatches);
  obj["UseStars"] = v.useStars;
}

template <>
void JSONObjectWriter::write(SEGMent::TextRiddle& v)
{
  v.question = obj["Question"].toString();
  v.expected = obj["Expected"].toString();
  v.ifCorrect = obj["IfCorrect"].toString();
  v.ifWrong = obj["IfWrong"].toString();
  const auto varlist = obj["FuzzyMatches"].toArray();
  for(const auto& val : varlist)
    v.fuzzyMatches.push_back(val.toString());
  v.useStars = obj["UseStars"].toBool();
}
