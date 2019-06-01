#pragma once
#include <score/command/Command.hpp>

#include <SEGMent/Commands/CommandFactory.hpp>
#include <SEGMent/Model/BackClickArea.hpp>
#include <SEGMent/Model/ClickArea.hpp>
#include <SEGMent/Model/GifObject.hpp>
#include <SEGMent/Model/ProcessModel.hpp>
#include <SEGMent/Model/Scene.hpp>
#include <SEGMent/Model/SimpleObject.hpp>
#include <SEGMent/Model/Sound.hpp>
#include <SEGMent/Model/TextArea.hpp>
#include <SEGMent/Model/Transition.hpp>
namespace SEGMent
{

template <typename T, auto Sound = nullptr>
struct SetVolume : score::Command
{
public:
  const CommandGroupKey& parentKey() const noexcept override
  {
    return CommandFactoryName();
  }

  static const CommandKey& static_key() noexcept
  {
    auto name = QString("SetVolume_") + Metadata<ObjectKey_k, T>::get();
    static const CommandKey kagi{std::move(name)};
    return kagi;
  }

  const CommandKey& key() const noexcept override { return static_key(); }

  QString description() const override
  {
    return QObject::tr("Set volume %1").arg(Metadata<Description_k, T>::get());
  }

  SetVolume() = default;

  SetVolume(const T& obj, double vol)
      : m_model{obj}
      , m_old(((const_cast<T&>(obj).*Sound)()).volume())
      , m_new{vol}
  {
  }

  void undo(const score::DocumentContext& ctx) const override
  {
    auto& obj = m_model.find(ctx);
    ((obj.*Sound)()).setVolume(m_old);
  }

  void redo(const score::DocumentContext& ctx) const override
  {
    auto& obj = m_model.find(ctx);
    ((obj.*Sound)()).setVolume(m_new);
  }

  void update(const T& obj, double vol)
  {
    m_new = vol;
    ((const_cast<T&>(obj).*Sound)()).setVolume(m_new);
  }

  void serializeImpl(DataStreamInput& s) const override
  {
    s << m_model << m_old << m_new;
  }

  void deserializeImpl(DataStreamOutput& s) override
  {
    s >> m_model >> m_old >> m_new;
  }

private:
  Path<T> m_model;
  double m_old{};
  double m_new{};
};

template <typename T>
struct SoundAccessor
{
  static constexpr auto value = &T::sound_ref;
};

template <>
struct SoundAccessor<SceneModel>
{
  static constexpr auto value = &SceneModel::ambience_ref;
};
template <typename T>
struct SetVolume<T, nullptr> : SetVolume<T, SoundAccessor<T>::value>
{
  using SetVolume<T, SoundAccessor<T>::value>::SetVolume;
};
} // namespace SEGMent

SCORE_COMMAND_DECL_T(SEGMent::SetVolume<SEGMent::ImageModel>)
SCORE_COMMAND_DECL_T(SEGMent::SetVolume<SEGMent::GifModel>)
SCORE_COMMAND_DECL_T(SEGMent::SetVolume<SEGMent::ClickAreaModel>)
SCORE_COMMAND_DECL_T(SEGMent::SetVolume<SEGMent::BackClickAreaModel>)
SCORE_COMMAND_DECL_T(SEGMent::SetVolume<SEGMent::TextAreaModel>)
SCORE_COMMAND_DECL_T(SEGMent::SetVolume<SEGMent::SceneModel>)
SCORE_COMMAND_DECL_T(SEGMent::SetVolume<SEGMent::TransitionModel>)
