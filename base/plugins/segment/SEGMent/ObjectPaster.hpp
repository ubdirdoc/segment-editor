#pragma once
#include <SEGMent/Document.hpp>
#include <SEGMent/Commands/Creation.hpp>

#include <core/document/Document.hpp>

#include <QMimeData>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QGraphicsView>

namespace SEGMent
{
/**
 * @brief Handles the pasting logic
 */
struct ObjectPaster
{
  score::Document& doc;
  DocumentView& view;
  QPoint widget_pos;
  QPointF scene_pos;

  void paste(const QMimeData* m)
  {
    if (m->hasFormat("segment/scenes"))
    {
      pasteScenes(m);
    }
    else if (m->hasFormat("segment/objects"))
    {
      pasteObjects(m);
    }
  }

  static void copyResources(QString origDir, QString thisDir, QJsonArray resources)
  {
    for(auto res : resources)
    {
      const auto& file = res.toString();
      QFile f(origDir + QDir::separator() + file);
      f.copy(thisDir + QDir::separator() + file);
    }
  }

  void pasteScenes(const QMimeData* m)
  {
    auto json = QJsonDocument::fromBinaryData(m->data("segment/scenes"));
    auto& model = score::IDocument::get<SEGMent::DocumentModel>(doc);
    auto& proc = model.process();

    auto obj = json.object();

    // Copy the resources from the copied-from project folder to the pasted-to project folder
    const auto& docPath = obj["DocumentPath"].toString();
    const auto& docResources = obj["Resources"].toArray();
    copyResources(QFileInfo{docPath}.absolutePath(), QFileInfo{doc.metadata().fileName()}.absolutePath(), docResources);

    std::vector<Id<SceneModel>> ids;
    for(auto& scene : proc.scenes) ids.push_back(scene.id());

    std::unordered_map<QString, Id<SceneModel>> id_map;

    // Find the center of all the scenes
    QPointF topLeft(INT_MAX, INT_MAX);
    QPointF bottomRight(-INT_MAX, -INT_MAX);
    for(const auto& scene_ref : obj["Scenes"].toArray())
    {
      auto scene = scene_ref.toObject();
      auto path = scene["Path"].toString();

      // Also generate new ids for the scenes
      auto new_id = getStrongId(ids);
      id_map[path] = new_id;
      ids.push_back(new_id);

      auto rect = fromJsonValue<QRectF>(scene["Rect"]);
      if(rect.x() < topLeft.x()) topLeft.setX(rect.x());
      if(rect.y() < topLeft.y()) topLeft.setY(rect.y());
      if(rect.x() + rect.width() > bottomRight.x()) bottomRight.setX(rect.x() + rect.width());
      if(rect.y() + rect.height() > bottomRight.y()) bottomRight.setY(rect.y() + rect.height());
    }
    const QPointF orig_center = QRectF(topLeft, bottomRight).center();

    // Create all the commands
    RedoMacroCommandDispatcher<PasteScenes> disp{doc.context().commandStack};
    for(auto scene_ref : obj["Scenes"].toArray())
    {
      auto scene = scene_ref.toObject();
      auto rect = fromJsonValue<QRectF>(scene["Rect"]);
      auto path = scene["Path"].toString();

      scene["Rect"] = toJsonValue(
            QRectF{rect.x() - orig_center.x() + scene_pos.x(),
                   rect.y() - orig_center.y() + scene_pos.y(),
                   rect.width(), rect.height()});
      disp.submitCommand(new PasteScene{proc, scene, id_map.at(path)});
    }

    // This is the complicated part : we have to adjust the paths stored in the
    // transitions, so that they point to the pasted objects instead of the original ones.
    // This means that we have to mess with the JSON data a little bit.
    for(auto transition_ref : obj["Transitions"].toArray())
    {
      auto transition = transition_ref.toObject();
      auto trans_sub = transition["Transition"].toObject();
      auto which_trans = trans_sub["Which"].toString();
      auto trans_object = trans_sub[which_trans].toObject();
      auto to = trans_object["To"].toString();
      auto to_newPath = pathToString(Path{proc.scenes.at(id_map[to])});
      trans_object["To"] = to_newPath;

      if(which_trans == "SceneToScene")
      {
        auto from = trans_object["From"].toString();
        auto from_newPath = pathToString(Path{proc.scenes.at(id_map[from])});
        trans_object["From"] = from_newPath;
      }
      else
      {
        auto from = trans_object["From"].toString();
        auto object_from = from.mid(from.lastIndexOf('/'));
        auto scene_from = from.mid(0, from.lastIndexOf('/'));
        auto from_newPath = pathToString(Path{proc.scenes.at(id_map[scene_from])}) + object_from;
        trans_object["From"] = from_newPath;
      }

      trans_sub[which_trans] = trans_object;
      transition["Transition"] = trans_sub;

      disp.submitCommand(new PasteTransition{proc, transition});
    }

    disp.commit();
  }

  /**
   * @brief Paste objects from a scene to another, e.g. gifs, text areas, etc.
   */
  void pasteObjects(const QMimeData* m)
  {
    // First locate the SceneWindow we are pasting into.
    auto item_under_mouse = view.graphicsView().itemAt(widget_pos);
    if (!item_under_mouse)
      return;

    while (item_under_mouse->type() != SEGMent::SceneWindow::static_type())
    {
      item_under_mouse = item_under_mouse->parentItem();
      if (!item_under_mouse)
        return;
    }

    auto& scene = static_cast<SceneWindow*>(item_under_mouse)->model();

    auto item_pos = item_under_mouse->mapFromScene(scene_pos);
    QPointF relative_item_pos = {
      item_pos.x() / item_under_mouse->boundingRect().width(),
      item_pos.y() / item_under_mouse->boundingRect().height(),
    };

    // Then for each copied object add a new pasting command with the object data.
    RedoMacroCommandDispatcher<PasteObjects> disp{doc.context().commandStack};
    auto arr = QJsonDocument::fromBinaryData(m->data("segment/objects")).array();
    for(const auto json : arr)
    {
      auto obj = json.toObject();
      obj["Pos"] = toJsonValue(relative_item_pos);
      if (obj["ObjectKind"] == Metadata<ObjectKey_k, ImageModel>::get())
      {
        disp.submitCommand(new PasteObject<ImageModel>(scene, obj));
      }
      else if (obj["ObjectKind"] == Metadata<ObjectKey_k, GifModel>::get())
      {
        disp.submitCommand(new PasteObject<GifModel>(scene, obj));
      }
      else if (obj["ObjectKind"] == Metadata<ObjectKey_k, ClickAreaModel>::get())
      {
        disp.submitCommand(new PasteObject<ClickAreaModel>(scene, obj));
      }
      else if (obj["ObjectKind"]
               == Metadata<ObjectKey_k, BackClickAreaModel>::get())
      {
        disp.submitCommand(new PasteObject<BackClickAreaModel>(scene, obj));
      }
      else if (obj["ObjectKind"] == Metadata<ObjectKey_k, TextAreaModel>::get())
      {
        disp.submitCommand(new PasteObject<TextAreaModel>(scene, obj));
      }
    }
    disp.commit();
  }
};
}
