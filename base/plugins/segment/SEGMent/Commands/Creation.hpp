#pragma once
#include <QUrl>
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
#include <score/command/Command.hpp>
#include <score/command/PropertyCommand.hpp>
#include <score/selection/SelectionStack.hpp>
#include <score/document/DocumentContext.hpp>
#include <score/model/path/PathSerialization.hpp>
#include <score/tools/IdentifierGeneration.hpp>
namespace SEGMent
{

class PasteScene final
    : public score::Command
{
  SCORE_COMMAND_DECL(CommandFactoryName(), PasteScene, "Paste a scene")
public:
  PasteScene(const ProcessModel& process, QJsonObject obj)
      : m_path{process}
      , m_newId{getStrongId(process.scenes)}
      , m_json{std::move(obj)}
  {
  }

  void undo(const score::DocumentContext& ctx) const override
  {
    auto& process = m_path.find(ctx);
    process.scenes.remove(m_newId);
  }

  void redo(const score::DocumentContext& ctx) const override
  {
    auto& process = m_path.find(ctx);
    auto scene = new SEGMent::SceneModel{JSONObjectWriter{m_json}, &process};
    scene->setId(getStrongId(process.scenes));
    process.scenes.add(scene);
  }

protected:
  void serializeImpl(DataStreamInput& s) const override
  {
    s << m_path << m_newId << m_json;
  }
  void deserializeImpl(DataStreamOutput& s) override
  {
    s >> m_path >> m_newId >> m_json;
  }

private:
  Path<ProcessModel> m_path;
  Id<SceneModel> m_newId{};
  QJsonObject m_json{};
};

template<typename T>
class PasteObject final
    : public score::Command
{
public:
    PasteObject() = default;
    const CommandGroupKey& parentKey() const noexcept override
    {
      return CommandFactoryName();
    }

    static const CommandKey& static_key() noexcept
    {
      auto name = QString("PasteObject_") + Metadata<ObjectKey_k, T>::get();
      static const CommandKey kagi{std::move(name)};
      return kagi;
    }

    const CommandKey& key() const noexcept override
    {
      return static_key();
    }

    QString description() const override
    {
      return QObject::tr("Paste %1")
          .arg(Metadata<Description_k, T>::get());
    }
    PasteObject(const SceneModel& scene, QJsonObject obj)
      : m_path{scene}
      , m_newId{getStrongId(SceneAccessor<T>::get(scene))}
      , m_json{std::move(obj)}
    {
    }

  void undo(const score::DocumentContext& ctx) const override
  {
    auto& scene = m_path.find(ctx);
    SceneAccessor<T>::get(scene).remove(m_newId);
  }

  void redo(const score::DocumentContext& ctx) const override
  {
    auto& scene = m_path.find(ctx);
    auto obj = new T{JSONObjectWriter{m_json}, &scene};
    obj->setId(getStrongId(SceneAccessor<T>::get(scene)));
    SceneAccessor<T>::get(scene).add(obj);
  }

protected:
  void serializeImpl(DataStreamInput& s) const override
  {
    s << m_path << m_newId << m_json;
  }
  void deserializeImpl(DataStreamOutput& s) override
  {
    s >> m_path >> m_newId >> m_json;
  }

private:
  Path<SceneModel> m_path;
  Id<T> m_newId{};
  QJsonObject m_json{};
};


//! Used when a scene is dropped from the library on the canvas
class DropScene final
    : public score::Command
{
  SCORE_COMMAND_DECL(CommandFactoryName(), DropScene, "Add a scene")
public:
  DropScene(const ProcessModel& process, Image image, QRectF rect)
      : m_path{process}
      , m_newId{getStrongId(process.scenes)}
      , m_image{image}
      , m_rect{rect}
  {
  }

  void undo(const score::DocumentContext& ctx) const override
  {
    auto& process = m_path.find(ctx);
    process.scenes.remove(m_newId);
  }

  void redo(const score::DocumentContext& ctx) const override
  {
    auto& process = m_path.find(ctx);
    auto scene = new SceneModel{m_newId, &process};
    scene->setImage(m_image);
    scene->setRect(m_rect);
    scene->metadata().setLabel(QUrl(m_image).fileName().split(".")[0]);
    process.scenes.add(scene);
  }

protected:
  void serializeImpl(DataStreamInput& s) const override
  {
    s << m_path << m_newId << m_image << m_rect;
  }
  void deserializeImpl(DataStreamOutput& s) override
  {
    s >> m_path >> m_newId >> m_image >> m_rect;
  }

private:
  Path<ProcessModel> m_path;
  Id<SceneModel> m_newId{};
  Image m_image{};
  QRectF m_rect;
};

//! Used when a simple image is dropped from the library on the scene
class DropImage final
    : public score::Command
{
  SCORE_COMMAND_DECL(CommandFactoryName(), DropImage, "Add an image")
public:
  DropImage(const SceneModel& obj, Image image, QPointF pos, QSizeF size)
      : m_path{obj}
      , m_newId{getStrongId(obj.objects)}
      , m_image{image}
      , m_pos{pos}
      , m_size{size}
  {
  }

  void undo(const score::DocumentContext& ctx) const override
  {
    auto& scene = m_path.find(ctx);
    scene.objects.remove(m_newId);
  }

  void redo(const score::DocumentContext& ctx) const override
  {
    auto& scene = m_path.find(ctx);
    auto obj = new ImageModel{m_newId, &scene};
    obj->setImage(m_image);
    obj->setPos(m_pos);
    obj->setSize(m_size);
    scene.objects.add(obj);
  }

protected:
  void serializeImpl(DataStreamInput& s) const override
  {
    s << m_path << m_newId << m_image << m_pos << m_size;
  }
  void deserializeImpl(DataStreamOutput& s) override
  {
    s >> m_path >> m_newId >> m_image >> m_pos >> m_size;
  }

private:
  Path<SceneModel> m_path;
  Id<ImageModel> m_newId{};
  Image m_image;
  QPointF m_pos{};
  QSizeF m_size{};
};

//! Used when a gif is dropped from the library on the scene
class DropGif final
    : public score::Command
{
  SCORE_COMMAND_DECL(CommandFactoryName(), DropGif, "Add a gif")
public:
  DropGif(const SceneModel& obj, Image image, QPointF pos, QSizeF sz)
      : m_path{obj}
      , m_newId{getStrongId(obj.gifs)}
      , m_image{image}
      , m_pos{pos}
      , m_size{sz}
  {
  }

  void undo(const score::DocumentContext& ctx) const override
  {
    auto& scene = m_path.find(ctx);
    scene.gifs.remove(m_newId);
  }

  void redo(const score::DocumentContext& ctx) const override
  {
    auto& scene = m_path.find(ctx);
    auto obj = new GifModel{m_newId, &scene};
    obj->setImage(m_image);
    obj->setPos(m_pos);
    obj->setSize(m_size);
    scene.gifs.add(obj);
  }

protected:
  void serializeImpl(DataStreamInput& s) const override
  {
    s << m_path << m_newId << m_image << m_pos << m_size;
  }
  void deserializeImpl(DataStreamOutput& s) override
  {
    s >> m_path >> m_newId >> m_image >> m_pos >> m_size;
  }

private:
  Path<SceneModel> m_path;
  Id<GifModel> m_newId{};
  Image m_image;
  QPointF m_pos{};
  QSizeF m_size{};
};

//! Used when a click area is dropped from the library on the scene
class DropClickArea final
    : public score::Command
{
  SCORE_COMMAND_DECL(CommandFactoryName(), DropClickArea, "Add a click area")
public:
  DropClickArea(const SceneModel& scene, QPointF pos, QSizeF sz)
      : m_path{scene}
      , m_newId{getStrongId(scene.clickAreas)}
      , m_pos{pos}
      , m_size{sz}
  {
  }

  void undo(const score::DocumentContext& ctx) const override
  {
    auto& scene = m_path.find(ctx);
    scene.clickAreas.remove(m_newId);
  }

  void redo(const score::DocumentContext& ctx) const override
  {
    auto& scene = m_path.find(ctx);
    auto obj = new ClickAreaModel{m_newId, &scene};
    obj->setPos(m_pos);
    obj->setSize(m_size);
    scene.clickAreas.add(obj);
  }

protected:
  void serializeImpl(DataStreamInput& s) const override
  {
    s << m_path << m_newId << m_pos << m_size;
  }
  void deserializeImpl(DataStreamOutput& s) override
  {
    s >> m_path >> m_newId >> m_pos >> m_size;
  }

private:
  Path<SceneModel> m_path;
  Id<ClickAreaModel> m_newId{};
  QPointF m_pos;
  QSizeF m_size;
};

//! Used when a back-click area is dropped from the library on the scene
class DropBackClickArea final
    : public score::Command
{
  SCORE_COMMAND_DECL(CommandFactoryName(), DropBackClickArea, "Add a back click area")
public:
  DropBackClickArea(const SceneModel& scene, QPointF pos, QSizeF sz)
      : m_path{scene}
      , m_newId{getStrongId(scene.backClickAreas)}
      , m_pos{pos}
      , m_size{sz}
  {
  }

  void undo(const score::DocumentContext& ctx) const override
  {
    auto& scene = m_path.find(ctx);
    scene.backClickAreas.remove(m_newId);
  }

  void redo(const score::DocumentContext& ctx) const override
  {
    auto& scene = m_path.find(ctx);
    auto obj = new BackClickAreaModel{m_newId, &scene};
    obj->setPos(m_pos);
    obj->setSize(m_size);
    scene.backClickAreas.add(obj);
  }

protected:
  void serializeImpl(DataStreamInput& s) const override
  {
    s << m_path << m_newId << m_pos << m_size;
  }
  void deserializeImpl(DataStreamOutput& s) override
  {
    s >> m_path >> m_newId >> m_pos >> m_size;
  }

private:
  Path<SceneModel> m_path;
  Id<BackClickAreaModel> m_newId{};
  QPointF m_pos;
  QSizeF m_size;
};

//! Used when a text area is dropped from the library on the scene
class DropTextArea final
    : public score::Command
{
  SCORE_COMMAND_DECL(CommandFactoryName(), DropTextArea, "Add a text area")
public:
  DropTextArea(const SceneModel& scene, QPointF pos, QSizeF sz)
      : m_path{scene}
      , m_newId{getStrongId(scene.textAreas)}
      , m_pos{pos}
      , m_size{sz}
  {
  }

  void undo(const score::DocumentContext& ctx) const override
  {
    auto& scene = m_path.find(ctx);
    scene.textAreas.remove(m_newId);
  }

  void redo(const score::DocumentContext& ctx) const override
  {
    auto& scene = m_path.find(ctx);
    auto obj = new TextAreaModel{m_newId, &scene};
    obj->setPos(m_pos);
    obj->setSize(m_size);
    scene.textAreas.add(obj);
  }

protected:
  void serializeImpl(DataStreamInput& s) const override
  {
    s << m_path << m_newId << m_pos << m_size;
  }
  void deserializeImpl(DataStreamOutput& s) override
  {
    s >> m_path >> m_newId >> m_pos >> m_size;
  }

private:
  Path<SceneModel> m_path;
  Id<TextAreaModel> m_newId{};
  QPointF m_pos;
  QSizeF m_size;
};

//! Used when creating a transition between any two objects
class CreateTransition final
    : public score::Command
{
  SCORE_COMMAND_DECL(
      CommandFactoryName(), CreateTransition, "Add a transition")
public:
  CreateTransition(const ProcessModel& obj, transition_t trans)
      : m_path{obj}, m_newId{getStrongId(obj.transitions)}, m_trans{trans}
  {
  }

  void undo(const score::DocumentContext& ctx) const override
  {
    auto& scene = m_path.find(ctx);
    scene.transitions.remove(m_newId);
  }

  void redo(const score::DocumentContext& ctx) const override
  {
    auto& scene = m_path.find(ctx);
    auto obj = new TransitionModel{m_trans, m_newId, &scene};
    scene.transitions.add(obj);
  }

protected:
  void serializeImpl(DataStreamInput& s) const override
  {
    s << m_path << m_newId << m_trans;
  }
  void deserializeImpl(DataStreamOutput& s) override
  {
    s >> m_path >> m_newId >> m_trans;
  }

private:
  Path<ProcessModel> m_path;
  Id<TransitionModel> m_newId{};
  transition_t m_trans;
};

}

  SCORE_COMMAND_DECL_T(SEGMent::PasteObject<SEGMent::ImageModel>)
  SCORE_COMMAND_DECL_T(SEGMent::PasteObject<SEGMent::GifModel>)
  SCORE_COMMAND_DECL_T(SEGMent::PasteObject<SEGMent::ClickAreaModel>)
  SCORE_COMMAND_DECL_T(SEGMent::PasteObject<SEGMent::BackClickAreaModel>)
  SCORE_COMMAND_DECL_T(SEGMent::PasteObject<SEGMent::TextAreaModel>)
