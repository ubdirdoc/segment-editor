#pragma once
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
#include <score/command/AggregateCommand.hpp>
#include <score/command/Command.hpp>
#include <score/command/PropertyCommand.hpp>
#include <score/selection/SelectionStack.hpp>
#include <score/document/DocumentContext.hpp>
#include <score/model/path/PathSerialization.hpp>
#include <score/tools/IdentifierGeneration.hpp>

namespace SEGMent
{

//! Return true if p is the beginning of c, e.g. is_prefix({3, 5}, {3, 5, 7}) == true
template<typename T>
bool is_prefix(const std::vector<T>& p, const std::vector<T>& c)
{
  if(p.size() > c.size())
    return false;

  for(std::size_t i = 0; i < p.size(); i++)
  {
    if(!(p[i] == c[i]))
      return false;
  }

  return true;
}

//! Return true if the second path is a child object of the first
template<typename Parent, typename Child>
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
      : m_path{obj}
      , m_id{sc.id()}, m_scene{score::marshall<DataStream>(sc)}
    {
      auto sc_path = score::IDocument::path(sc);
      for(auto& trans : obj.transitions)
      {
        eggs::variants::apply([&] (const auto& transition) {
          if(is_parent(sc_path, transition.from) || is_parent(sc_path, transition.to))
          {
            m_transitions.emplace_back(trans.id(), score::marshall<DataStream>(trans));
          }
        }, trans.transition());
      }
    }

  void undo(const score::DocumentContext& ctx) const override
  {
    auto& proc = m_path.find(ctx);

    proc.scenes.add(new SceneModel{DataStreamWriter{m_scene}, &proc});
    for(auto& trans : m_transitions)
    {
      proc.transitions.add(new TransitionModel{DataStreamWriter{trans.second}, &proc});
    }
  }

  void redo(const score::DocumentContext& ctx) const override
  {
    auto& proc = m_path.find(ctx);
    ctx.selectionStack.deselect();

    for(auto& trans : m_transitions)
      proc.transitions.remove(trans.first);

    proc.scenes.remove(m_id);
  }

protected:
  void serializeImpl(DataStreamInput& s) const override
  {
    s << m_path << m_id << m_scene << m_transitions;
  }
  void deserializeImpl(DataStreamOutput& s) override
  {
    s >> m_path >> m_id >> m_scene >> m_transitions;
  }

private:
  Path<ProcessModel> m_path;
  Id<SceneModel> m_id{};
  QByteArray m_scene;
  std::vector<std::pair<Id<TransitionModel>, QByteArray>> m_transitions;
};

class RemoveTransition : public score::Command
{
  SCORE_COMMAND_DECL(CommandFactoryName(), RemoveTransition, "Remove a transition")
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
    proc.transitions.add(new TransitionModel{DataStreamWriter{m_trans}, &proc});
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

template<typename T>
class RemoveObject: public score::Command
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

    const CommandKey& key() const noexcept override
    {
      return static_key();
    }

    QString description() const override
    {
      return QObject::tr("Remove %1")
          .arg(Metadata<Description_k, T>::get());
    }

    RemoveObject(const SceneModel& scene, const T& object)
      : m_path{scene}
      , m_id{object.id()}
      , m_object{score::marshall<DataStream>(object)}
    {
      auto& proc = (SEGMent::ProcessModel&) *scene.parent();
      auto obj_path = score::IDocument::path(object);
      for(auto& trans : proc.transitions)
      {
        qDebug() << trans.transition().which();
        eggs::variants::apply([&] (const auto& transition) {
          if(is_parent(obj_path, transition.from) || is_parent(obj_path, transition.to))
          {
            m_transitions.emplace_back(trans.id(), score::marshall<DataStream>(trans));
          }
        }, trans.transition());
      }
    }

  void undo(const score::DocumentContext& ctx) const override
  {
    auto& scene = m_path.find(ctx);
    auto& proc = (SEGMent::ProcessModel&) *scene.parent();

    SceneAccessor<T>{}.get(scene).add(new T{DataStreamWriter{m_object}, &scene});
    for(auto& trans : m_transitions)
    {
      proc.transitions.add(new TransitionModel{DataStreamWriter{trans.second}, &proc});
    }
  }

  void redo(const score::DocumentContext& ctx) const override
  {
    auto& scene = m_path.find(ctx);
    auto& proc = (SEGMent::ProcessModel&) *scene.parent();
    ctx.selectionStack.deselect();

    for(auto& trans : m_transitions)
      proc.transitions.remove(trans.first);

    SceneAccessor<T>{}.get(scene).remove(m_id);
  }

protected:
  void serializeImpl(DataStreamInput& s) const override
  {
    s << m_path << m_id << m_object << m_transitions;
  }
  void deserializeImpl(DataStreamOutput& s) override
  {
    s >> m_path >> m_id >> m_object >> m_transitions;
  }

private:
  Path<SceneModel> m_path;
  Id<T> m_id{};
  QByteArray m_object;
  std::vector<std::pair<Id<TransitionModel>, QByteArray>> m_transitions;
};

struct RemoveObjectVisitor
{
  score::Command* operator()(const SceneModel& obj) const
  {
    return new RemoveScene{*static_cast<ProcessModel*>(obj.parent()), obj};
  }
  score::Command* operator()(const TransitionModel& obj) const
  {
    return new RemoveTransition{*static_cast<ProcessModel*>(obj.parent()), obj};
  }
  template<typename T>
  score::Command* operator()(const T& obj) const
  {
    return new RemoveObject{*static_cast<SceneModel*>(obj.parent()), obj};
  }
};

struct RemoveObjects
    : public score::AggregateCommand
{
  SCORE_COMMAND_DECL(CommandFactoryName(), RemoveObjects, "Remove objects")
};
}

SCORE_COMMAND_DECL_T(SEGMent::RemoveObject<SEGMent::ImageModel>)
SCORE_COMMAND_DECL_T(SEGMent::RemoveObject<SEGMent::GifModel>)
SCORE_COMMAND_DECL_T(SEGMent::RemoveObject<SEGMent::ClickAreaModel>)
SCORE_COMMAND_DECL_T(SEGMent::RemoveObject<SEGMent::BackClickAreaModel>)
SCORE_COMMAND_DECL_T(SEGMent::RemoveObject<SEGMent::TextAreaModel>)
