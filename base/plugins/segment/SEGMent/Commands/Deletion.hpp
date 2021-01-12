#pragma once
#include <score/command/AggregateCommand.hpp>
#include <score/command/Command.hpp>
#include <score/command/PropertyCommand.hpp>
#include <score/command/Dispatchers/MacroCommandDispatcher.hpp>
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
#include <SEGMent/Model/ImageModel.hpp>
#include <SEGMent/Model/Sound.hpp>
#include <SEGMent/Model/TextArea.hpp>
#include <SEGMent/Model/Transition.hpp>
#include <unordered_map>

namespace SEGMent
{

//! Return true if p is the beginning of c, e.g. is_prefix({3, 5}, {3, 5, 7})
//! == true
template <typename T>
bool is_prefix(const std::vector<T>& p, const std::vector<T>& c)
{
  if (p.size() > c.size())
    return false;

  for (std::size_t i = 0; i < p.size(); i++)
  {
    if (!(p[i] == c[i]))
      return false;
  }

  return true;
}

//! Return true if the second path is a child object of the first
template <typename Parent, typename Child>
bool is_parent(const Parent& parent, const Child& child)
{
  const auto& p = parent.unsafePath();
  const auto& c = child.unsafePath();
  return is_prefix(p.vec(), c.vec());
}

class RemoveScene : public score::Command
{
  SCORE_COMMAND_DECL(CommandFactoryName(), RemoveScene, "Remove a scene")
public:
  RemoveScene(const ProcessModel& obj, const SceneModel& sc)
      : m_path{obj}, m_id{sc.id()}, m_scene{score::marshall<DataStream>(sc)}
  {
  }

  void undo(const score::DocumentContext& ctx) const override
  {
    auto& proc = m_path.find(ctx);

    proc.scenes.add(new SceneModel{DataStreamWriter{m_scene}, &proc});
  }

  void redo(const score::DocumentContext& ctx) const override
  {
    auto& proc = m_path.find(ctx);
    ctx.selectionStack.deselect();

    proc.scenes.remove(m_id);
  }

protected:
  void serializeImpl(DataStreamInput& s) const override
  {
    s << m_path << m_id << m_scene;
  }
  void deserializeImpl(DataStreamOutput& s) override
  {
    s >> m_path >> m_id >> m_scene;
  }

private:
  Path<ProcessModel> m_path;
  Id<SceneModel> m_id{};
  QByteArray m_scene;
};

class RemoveTransition : public score::Command
{
  SCORE_COMMAND_DECL(
      CommandFactoryName(),
      RemoveTransition,
      "Remove a transition")
public:
  RemoveTransition(const ProcessModel& obj, const TransitionModel& trans)
      : m_path{obj}
      , m_id{trans.id()}
      , m_trans{score::marshall<DataStream>(trans)}
  {
  }

  void undo(const score::DocumentContext& ctx) const override
  {
    auto& proc = m_path.find(ctx);
    proc.transitions.add(
        new TransitionModel{DataStreamWriter{m_trans}, &proc});
  }

  void redo(const score::DocumentContext& ctx) const override
  {
    auto& proc = m_path.find(ctx);
    ctx.selectionStack.deselect();
    proc.transitions.remove(m_id);
  }

protected:
  void serializeImpl(DataStreamInput& s) const override
  {
    s << m_path << m_id << m_trans;
  }
  void deserializeImpl(DataStreamOutput& s) override
  {
    s >> m_path >> m_id >> m_trans;
  }

private:
  Path<ProcessModel> m_path;
  Id<TransitionModel> m_id{};
  QByteArray m_trans;
};

template <typename T>
class RemoveObject : public score::Command
{
public:
  RemoveObject() = default;
  const CommandGroupKey& parentKey() const noexcept override
  {
    return CommandFactoryName();
  }

  static const CommandKey& static_key() noexcept
  {
    auto name = QString("RemoveObject_") + Metadata<ObjectKey_k, T>::get();
    static const CommandKey kagi{std::move(name)};
    return kagi;
  }

  const CommandKey& key() const noexcept override { return static_key(); }

  QString description() const override
  {
    return QObject::tr("Remove %1").arg(Metadata<Description_k, T>::get());
  }

  RemoveObject(const SceneModel& scene, const T& object)
      : m_path{scene}
      , m_id{object.id()}
      , m_object{score::marshall<DataStream>(object)}
  {
  }

  void undo(const score::DocumentContext& ctx) const override
  {
    auto& scene = m_path.find(ctx);

    SceneAccessor<T>{}.get(scene).add(
        new T{DataStreamWriter{m_object}, &scene});
  }

  void redo(const score::DocumentContext& ctx) const override
  {
    auto& scene = m_path.find(ctx);
    ctx.selectionStack.deselect();

    SceneAccessor<T>{}.get(scene).remove(m_id);
  }

protected:
  void serializeImpl(DataStreamInput& s) const override
  {
    s << m_path << m_id << m_object;
  }
  void deserializeImpl(DataStreamOutput& s) override
  {
    s >> m_path >> m_id >> m_object;
  }

private:
  Path<SceneModel> m_path;
  Id<T> m_id{};
  QByteArray m_object;
};

struct RemoveObjects : public score::AggregateCommand
{
  SCORE_COMMAND_DECL(CommandFactoryName(), RemoveObjects, "Remove objects")
};

struct RemoveObjectVisitor
{
  MacroCommandDispatcher<RemoveObjects>& dispatcher;
  std::unordered_map<Id<TransitionModel>, score::Command*> transitions;
  std::vector<score::Command*> others;
  void operator()(const SceneModel& sc)
  {
      auto& proc = *static_cast<ProcessModel*>(sc.parent());
      auto sc_path = score::IDocument::path(sc);
      for (auto& trans : proc.transitions)
      {
        eggs::variants::apply(
            [&](const auto& transition) {
              if (is_parent(sc_path, transition.from)
                  || is_parent(sc_path, transition.to))
              {
                  if(transitions.find(trans.id()) == transitions.end())
                      transitions[trans.id()] = new RemoveTransition{proc, trans};
              }
            },
            trans.transition());
      }

      others.push_back(new RemoveScene{proc, sc});
  }
  void operator()(const TransitionModel& trans)
  {
      auto& proc = *static_cast<ProcessModel*>(trans.parent());
      if(transitions.find(trans.id()) == transitions.end())
          transitions[trans.id()] = new RemoveTransition{proc, trans};
  }
  template <typename T>
  void operator()(const T& obj)
  {
    auto& scene = *static_cast<SceneModel*>(obj.parent());
    auto& proc = (SEGMent::ProcessModel&)*scene.parent();
    auto obj_path = score::IDocument::path(obj);
    for (auto& trans : proc.transitions)
    {
      eggs::variants::apply(
          [&](const auto& transition) {
            if (is_parent(obj_path, transition.from)
                || is_parent(obj_path, transition.to))
            {
                if(transitions.find(trans.id()) == transitions.end())
                    transitions[trans.id()] = new RemoveTransition{proc, trans};
            }
          },
          trans.transition());
    }
    others.push_back(new RemoveObject{scene, obj});
  }

  void finish()
  {
      for(auto& [id, cmd] : transitions)
          dispatcher.submitCommand(cmd);
      for(auto& cmd : others)
          dispatcher.submitCommand(cmd);
  }
};

} // namespace SEGMent

SCORE_COMMAND_DECL_T(SEGMent::RemoveObject<SEGMent::ImageModel>)
SCORE_COMMAND_DECL_T(SEGMent::RemoveObject<SEGMent::GifModel>)
SCORE_COMMAND_DECL_T(SEGMent::RemoveObject<SEGMent::ClickAreaModel>)
SCORE_COMMAND_DECL_T(SEGMent::RemoveObject<SEGMent::BackClickAreaModel>)
SCORE_COMMAND_DECL_T(SEGMent::RemoveObject<SEGMent::TextAreaModel>)
