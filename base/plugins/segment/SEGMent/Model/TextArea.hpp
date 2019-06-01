#pragma once
#include <score/model/Entity.hpp>
#include <score/selection/Selectable.hpp>

#include <SEGMent/Model/Model.hpp>
#include <SEGMent/Model/Traits.hpp>
namespace SEGMent
{

class TextAreaModel : public Object<TextAreaModel>
{
  W_OBJECT(TextAreaModel)
  SCORE_SERIALIZE_FRIENDS
public:
  using base_type = Object<TextAreaModel>;
  enum Behaviour
  {
    Add,
    Replace,
    Clear,
    Validate
  };
  Selectable selection{this};

  TextAreaModel(const Id<TextAreaModel>& id, QObject* parent);

  template <typename DeserializerVisitor>
  TextAreaModel(DeserializerVisitor&& vis, QObject* parent)
      : base_type{vis, parent}
  {
    vis.writeTo(*this);
  }

public:
  const QString& text() const MSVC_NOEXCEPT;
  void setText(const QString& v) MSVC_NOEXCEPT;
  void textChanged(const QString& v) W_SIGNAL(textChanged, v);
  PROPERTY(QString, text READ text WRITE setText NOTIFY textChanged)
private:
  QString m_text;

public:
  Behaviour behaviour() const MSVC_NOEXCEPT;
  void setBehaviour(Behaviour v) MSVC_NOEXCEPT;
  void behaviourChanged(Behaviour v) W_SIGNAL(behaviourChanged, v);
  PROPERTY(
      Behaviour,
      behaviour READ behaviour WRITE setBehaviour NOTIFY behaviourChanged)
private:
  Behaviour m_behaviour{};
};
} // namespace SEGMent

Q_DECLARE_METATYPE(SEGMent::TextAreaModel::Behaviour)
W_REGISTER_ARGTYPE(SEGMent::TextAreaModel::Behaviour)

DEFAULT_MODEL_METADATA(SEGMent::TextAreaModel, "TextArea")
TR_TEXT_METADATA(, SEGMent::TextAreaModel, PrettyName_k, "Text area")
