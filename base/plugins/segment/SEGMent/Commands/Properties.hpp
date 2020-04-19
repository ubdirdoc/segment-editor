#pragma once
#include <score/command/AggregateCommand.hpp>
#include <score/command/Command.hpp>
#include <score/command/PropertyCommand.hpp>
#include <score/document/DocumentContext.hpp>
#include <score/model/path/PathSerialization.hpp>
#include <score/selection/SelectionStack.hpp>
#include <score/tools/IdentifierGeneration.hpp>

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
class MoveSceneRects : public score::AggregateCommand
{
  SCORE_COMMAND_DECL(CommandFactoryName(), MoveSceneRects, "Move scenes")
};

class ChangeRiddle : public score::Command
{
  SCORE_COMMAND_DECL(CommandFactoryName(), ChangeRiddle, "Change a riddle")
public:
  ChangeRiddle(const TransitionModel& obj, riddle_t trans)
      : m_path{obj}, m_new{trans}
  {
    if (auto t = obj.transition().target<SceneToScene>())
    {
      m_old = t->riddle;
    }
  }

  void undo(const score::DocumentContext& ctx) const override
  {
    setRiddle(m_old, ctx);
  }

  void redo(const score::DocumentContext& ctx) const override
  {
    setRiddle(m_new, ctx);
  }

protected:
  void setRiddle(const riddle_t& r, const score::DocumentContext& ctx) const
  {
    auto& trans = m_path.find(ctx);
    if (auto t = trans.transition().target<SceneToScene>())
    {
      auto new_t = *t;
      new_t.riddle = r;
      trans.setTransition(new_t);
    }
  }

  void serializeImpl(DataStreamInput& s) const override
  {
    s << m_path << m_old << m_new;
  }
  void deserializeImpl(DataStreamOutput& s) override
  {
    s >> m_path >> m_old >> m_new;
  }

private:
  Path<TransitionModel> m_path;
  riddle_t m_old, m_new;
};

class SetSceneLabel : public score::Command
{
  SCORE_COMMAND_DECL(CommandFactoryName(), SetSceneLabel, "Set scene label")
public:
  SetSceneLabel(const SceneModel& scene, const QString& t)
      : m_main{scene}, m_old{scene.metadata().getLabel()}, m_new{t}
  {
  }

  void undo(const score::DocumentContext& ctx) const override
  {
    auto& scene = m_main.find(ctx);
    scene.metadata().setLabel(m_old);
  }

  void redo(const score::DocumentContext& ctx) const override
  {
    auto& scene = m_main.find(ctx);
    scene.metadata().setLabel(m_new);
  }

protected:
  void serializeImpl(DataStreamInput& s) const override
  {
    s << m_main << m_old << m_new;
  }
  void deserializeImpl(DataStreamOutput& s) override
  {
    s >> m_main >> m_old >> m_new;
  }

private:
  Path<SceneModel> m_main, m_other;
  QString m_old, m_new;
};

class SetSceneType : public score::Command
{
  SCORE_COMMAND_DECL(CommandFactoryName(), SetSceneType, "Set scene type")
public:
  SetSceneType(const SceneModel& scene, const SceneModel::SceneType t)
      : m_main{scene}, m_main_type{t}, m_main_old_type{scene.sceneType()}
  {
    switch (t)
    {
      case SceneModel::SceneType::Default:
      case SceneModel::SceneType::GameOver:
        break;
      case SceneModel::SceneType::Initial:
      case SceneModel::SceneType::Final:
      {
        const auto& obj = *safe_cast<ProcessModel*>(scene.parent());
        for (auto& other_s : obj.scenes)
        {
          if (other_s.sceneType() == t && &other_s != &scene)
          {
            m_other = other_s;
            m_other_old_type = t;
            break;
          }
        }
      }
    }
  }

  void undo(const score::DocumentContext& ctx) const override
  {
    auto& scene = m_main.find(ctx);
    scene.setSceneType(m_main_old_type);

    if (auto o = m_other.try_find(ctx))
    {
      o->setSceneType(m_other_old_type);
    }
  }

  void redo(const score::DocumentContext& ctx) const override
  {
    auto& scene = m_main.find(ctx);
    scene.setSceneType(m_main_type);

    if (auto o = m_other.try_find(ctx))
    {
      o->setSceneType(SceneModel::SceneType::Default);
    }
  }

protected:
  void serializeImpl(DataStreamInput& s) const override
  {
    s << m_main << m_other << m_main_type << m_main_old_type
      << m_other_old_type;
  }
  void deserializeImpl(DataStreamOutput& s) override
  {
    s >> m_main >> m_other >> m_main_type >> m_main_old_type
        >> m_other_old_type;
  }

private:
  Path<SceneModel> m_main, m_other;
  SceneModel::SceneType m_main_type{}, m_main_old_type{};
  SceneModel::SceneType m_other_old_type{};
};

} // namespace SEGMent

namespace score
{
template <>
struct PropertyCommand_T<SEGMent::SceneModel::p_sceneType>
{
  template <typename T>
  struct command
  {
    using type = SEGMent::SetSceneType;
  };
};
} // namespace score
PROPERTY_COMMAND_T(SEGMent, SetObjectImage, ImageModel::p_image, "Set image")
PROPERTY_COMMAND_T(SEGMent, SetObjectPos, ImageModel::p_pos, "Set pos")
PROPERTY_COMMAND_T(SEGMent, SetObjectSize, ImageModel::p_size, "Set size")
PROPERTY_COMMAND_T(SEGMent, SetObjectZ, ImageModel::p_z, "Set Z")
PROPERTY_COMMAND_T(SEGMent, SetObjectSound, ImageModel::p_sound, "Set sound")
PROPERTY_COMMAND_T(
    SEGMent,
    SetObjectPuzzle,
    ImageModel::p_puzzle,
    "Set puzzle")

PROPERTY_COMMAND_T(SEGMent, SetGifImage, GifModel::p_image, "Set image")
PROPERTY_COMMAND_T(SEGMent, SetGifPos, GifModel::p_pos, "Set pos")
PROPERTY_COMMAND_T(SEGMent, SetGifSize, GifModel::p_size, "Set size")
PROPERTY_COMMAND_T(SEGMent, SetGifZ, GifModel::p_z, "Set Z")
PROPERTY_COMMAND_T(SEGMent, SetGifSound, GifModel::p_sound, "Set sound")
PROPERTY_COMMAND_T(SEGMent, SetGifFrames, GifModel::p_frames, "Set frames")
PROPERTY_COMMAND_T(
    SEGMent,
    SetGifDefaultFrame,
    GifModel::p_defaultFrame,
    "Set default frame")

PROPERTY_COMMAND_T(SEGMent, SetClickAreaPos, ClickAreaModel::p_pos, "Set pos")
PROPERTY_COMMAND_T(
    SEGMent,
    SetClickAreaSize,
    ClickAreaModel::p_size,
    "Set size")
PROPERTY_COMMAND_T(SEGMent, SetClickAreaZ, ClickAreaModel::p_z, "Set Z")
PROPERTY_COMMAND_T(
    SEGMent,
    SetClickAreaSound,
    ClickAreaModel::p_sound,
    "Set sound")

PROPERTY_COMMAND_T(
    SEGMent,
    SetBackClickAreaPos,
    BackClickAreaModel::p_pos,
    "Set pos")
PROPERTY_COMMAND_T(
    SEGMent,
    SetBackClickAreaSize,
    BackClickAreaModel::p_size,
    "Set size")
PROPERTY_COMMAND_T(
    SEGMent,
    SetBackClickAreaZ,
    BackClickAreaModel::p_z,
    "Set Z")
PROPERTY_COMMAND_T(
    SEGMent,
    SetBackClickAreaSound,
    BackClickAreaModel::p_sound,
    "Set sound")

PROPERTY_COMMAND_T(SEGMent, SetTextAreaPos, TextAreaModel::p_pos, "Set pos")
PROPERTY_COMMAND_T(SEGMent, SetTextAreaSize, TextAreaModel::p_size, "Set size")
PROPERTY_COMMAND_T(SEGMent, SetTextAreaZ, TextAreaModel::p_z, "Set Z")
PROPERTY_COMMAND_T(SEGMent, SetTextAreaText, TextAreaModel::p_text, "Set text")
PROPERTY_COMMAND_T(
    SEGMent,
    SetTextAreaSound,
    TextAreaModel::p_sound,
    "Set sound")
PROPERTY_COMMAND_T(
    SEGMent,
    SetTextAreaBehaviour,
    TextAreaModel::p_behaviour,
    "Set behaviour")

PROPERTY_COMMAND_T(
    SEGMent,
    SetSceneAmbience,
    SceneModel::p_ambience,
    "Set ambience")
PROPERTY_COMMAND_T(
    SEGMent,
    SetSceneStartText,
    SceneModel::p_startText,
    "Set start text")
PROPERTY_COMMAND_T(
    SEGMent,
    SetSceneRepeatText,
    SceneModel::p_repeatText,
    "Set start text repeats")
PROPERTY_COMMAND_T(
    SEGMent,
    SetSceneSonar,
    SceneModel::p_sonar,
    "Enable or disable the sonar")
PROPERTY_COMMAND_T(SEGMent, SetSceneImage, SceneModel::p_image, "Set image")
PROPERTY_COMMAND_T(SEGMent, SetSceneRect, SceneModel::p_rect, "Set rect")
PROPERTY_COMMAND_T(SEGMent, SetSceneCue, SceneModel::p_cue, "Set cue")
PROPERTY_COMMAND_T(SEGMent, SetSceneJournal, SceneModel::p_journal, "Set journal")

PROPERTY_COMMAND_T(
    SEGMent,
    SetTransitionSound,
    TransitionModel::p_sound,
    "Set sound")
PROPERTY_COMMAND_T(
    SEGMent,
    SetTransitionFade,
    TransitionModel::p_fade,
    "Set fade")
PROPERTY_COMMAND_T(
    SEGMent,
    SetTransitionColor,
    TransitionModel::p_color,
    "Set color")
PROPERTY_COMMAND_T(
    SEGMent,
    SetTransitionUnique,
    TransitionModel::p_unique,
    "Set unique")
PROPERTY_COMMAND_T(
    SEGMent,
    SetTransitionEventToAdd,
    TransitionModel::p_eventToAdd,
    "Set event to add")
PROPERTY_COMMAND_T(
    SEGMent,
    SetTransitionEventToRemove,
    TransitionModel::p_eventToRemove,
    "Set event to remove")
PROPERTY_COMMAND_T(
    SEGMent,
    SetTransitionEventRequired,
    TransitionModel::p_eventRequired,
    "Set event required")

SCORE_COMMAND_DECL_T(SEGMent::SetObjectImage)
SCORE_COMMAND_DECL_T(SEGMent::SetObjectPos)
SCORE_COMMAND_DECL_T(SEGMent::SetObjectSize)
SCORE_COMMAND_DECL_T(SEGMent::SetObjectZ)
SCORE_COMMAND_DECL_T(SEGMent::SetObjectSound)
SCORE_COMMAND_DECL_T(SEGMent::SetObjectPuzzle)

SCORE_COMMAND_DECL_T(SEGMent::SetGifImage)
SCORE_COMMAND_DECL_T(SEGMent::SetGifPos)
SCORE_COMMAND_DECL_T(SEGMent::SetGifSize)
SCORE_COMMAND_DECL_T(SEGMent::SetGifZ)
SCORE_COMMAND_DECL_T(SEGMent::SetGifSound)
SCORE_COMMAND_DECL_T(SEGMent::SetGifFrames)
SCORE_COMMAND_DECL_T(SEGMent::SetGifDefaultFrame)

SCORE_COMMAND_DECL_T(SEGMent::SetClickAreaPos)
SCORE_COMMAND_DECL_T(SEGMent::SetClickAreaSize)
SCORE_COMMAND_DECL_T(SEGMent::SetClickAreaZ)
SCORE_COMMAND_DECL_T(SEGMent::SetClickAreaSound)

SCORE_COMMAND_DECL_T(SEGMent::SetBackClickAreaPos)
SCORE_COMMAND_DECL_T(SEGMent::SetBackClickAreaSize)
SCORE_COMMAND_DECL_T(SEGMent::SetBackClickAreaZ)
SCORE_COMMAND_DECL_T(SEGMent::SetBackClickAreaSound)

SCORE_COMMAND_DECL_T(SEGMent::SetTextAreaPos)
SCORE_COMMAND_DECL_T(SEGMent::SetTextAreaSize)
SCORE_COMMAND_DECL_T(SEGMent::SetTextAreaZ)
SCORE_COMMAND_DECL_T(SEGMent::SetTextAreaText)
SCORE_COMMAND_DECL_T(SEGMent::SetTextAreaSound)
SCORE_COMMAND_DECL_T(SEGMent::SetTextAreaBehaviour)

SCORE_COMMAND_DECL_T(SEGMent::SetSceneAmbience)
SCORE_COMMAND_DECL_T(SEGMent::SetSceneStartText)
SCORE_COMMAND_DECL_T(SEGMent::SetSceneRepeatText)
SCORE_COMMAND_DECL_T(SEGMent::SetSceneSonar)
SCORE_COMMAND_DECL_T(SEGMent::SetSceneImage)
SCORE_COMMAND_DECL_T(SEGMent::SetSceneRect)
SCORE_COMMAND_DECL_T(SEGMent::SetSceneCue)
SCORE_COMMAND_DECL_T(SEGMent::SetSceneJournal)

SCORE_COMMAND_DECL_T(SEGMent::SetTransitionSound)
SCORE_COMMAND_DECL_T(SEGMent::SetTransitionFade)
SCORE_COMMAND_DECL_T(SEGMent::SetTransitionColor)
SCORE_COMMAND_DECL_T(SEGMent::SetTransitionUnique)
SCORE_COMMAND_DECL_T(SEGMent::SetTransitionEventToAdd)
SCORE_COMMAND_DECL_T(SEGMent::SetTransitionEventToRemove)
SCORE_COMMAND_DECL_T(SEGMent::SetTransitionEventRequired)
