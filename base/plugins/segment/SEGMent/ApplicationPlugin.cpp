#include <SEGMent/ApplicationPlugin.hpp>
#include <score/actions/Menu.hpp>
#include <score/actions/MenuManager.hpp>
#include <score/command/Dispatchers/CommandDispatcher.hpp>
#include <score/command/Dispatchers/MacroCommandDispatcher.hpp>
#include <score/document/DocumentContext.hpp>
#include <score/document/DocumentInterface.hpp>
#include <score/plugins/documentdelegate/DocumentDelegateView.hpp>

#include <core/document/Document.hpp>
#include <core/document/DocumentModel.hpp>
#include <core/document/DocumentView.hpp>
#include <score/plugins/documentdelegate/DocumentDelegateView.hpp>

#include <QFileDialog>
#include <QApplication>
#include <QClipboard>
#include <QDialog>
#include <QFileInfo>
#include <QGraphicsView>
#include <QHBoxLayout>
#include <QHelpContentWidget>
#include <QHelpEngine>
#include <QHelpIndexWidget>
#include <QJsonDocument>
#include <QMainWindow>
#include <QMessageBox>
#include <QMimeData>
#include <QSaveFile>
#include <QSplitter>
#include <QTabWidget>
#include <QTextBrowser>
#include <QTranslator>
#include <QDesktopWidget>
#include <QDesktopServices>

#include <SEGMent/Document.hpp>
#include <SEGMent/Commands/Creation.hpp>
#include <SEGMent/Commands/Deletion.hpp>
#include <SEGMent/Items/ClickWindow.hpp>
#include <SEGMent/Items/ObjectWindow.hpp>
#include <SEGMent/Items/SceneWindow.hpp>
#include <SEGMent/Model/Layer/ProcessView.hpp>
#include <SEGMent/Model/Scene.hpp>
#include <SEGMent/Exporter.hpp>
#include <SEGMent/HelpWidget.hpp>
#include <SEGMent/ObjectCopier.hpp>
#include <SEGMent/ObjectPaster.hpp>
#include <SEGMent/SoundPlayer.hpp>
#include <SEGMent/Visitors.hpp>
#include <SEGMent/ZOrder.hpp>

#include <QProcess>

#include <boost/bimap/bimap.hpp>
#include <cmath>
#include <score_git_info.hpp>
#include <wobjectimpl.h>

#include <unordered_set>
#include <typeindex>

namespace SEGMent
{
//! Get the path to the application binary.
//! Used to find the Unity3D engine files.
QString applicationPath()
{
    auto segment_path = qApp->applicationDirPath();
#if defined(__APPLE__)
    QDir d{segment_path};
    d.cdUp();
    segment_path = d.path();
#endif
    return segment_path;
}



ApplicationPlugin::ApplicationPlugin(const score::GUIApplicationContext& presenter)
  : score::GUIApplicationPlugin{presenter}
{
  m_copy_act = new QAction(tr("Copy"), this);
  m_paste_act = new QAction(tr("Paste"), this);
  m_help_act = new QAction(tr("Help"), this);
  m_delete_act = new QAction(tr("Delete"), this);
  m_stop_sound = new QAction(tr("Stop sound"), this);

  /*
          m_moveAction = new QAction{tr("Move"), this};
          m_resizeAction = new QAction{tr("Resize"), this};
          m_arrowAction = new QAction{tr("Create arrows"), this};
    */
  m_copy_act->setShortcut(QKeySequence::Copy);
  m_paste_act->setShortcut(QKeySequence::Paste);
  m_help_act->setShortcut(QKeySequence::HelpContents);
  m_delete_act->setShortcut(QKeySequence::Delete);

  m_help_act->setShortcutContext(Qt::ApplicationShortcut);
  m_paste_act->setShortcutContext(Qt::WidgetShortcut);
  m_copy_act->setShortcutContext(Qt::WidgetShortcut);
  m_delete_act->setShortcutContext(Qt::WidgetShortcut);

  m_movefront = new QAction(tr("Move to front"), this);
  m_moveback = new QAction(tr("Move behind"), this);

  m_movefront->setShortcut(QKeySequence::MoveToNextChar);
  m_moveback->setShortcut(QKeySequence::MoveToPreviousChar);
  m_movefront->setShortcutContext(Qt::ApplicationShortcut);
  m_moveback->setShortcutContext(Qt::ApplicationShortcut);

  m_zoomplus = new QAction(tr("Zoom (+)"), this);
  m_zoomminus = new QAction(tr("Zoom (-)"), this);
  m_recenter = new QAction(tr("Recenter"), this);

  m_zoomplus->setShortcut(QKeySequence::ZoomIn);
  m_zoomminus->setShortcut(QKeySequence::ZoomOut);
  m_recenter->setShortcut(QKeySequence("CTRL + ALT + 0"));
  m_zoomplus->setShortcutContext(Qt::ApplicationShortcut);
  m_zoomminus->setShortcutContext(Qt::ApplicationShortcut);
  m_recenter->setShortcutContext(Qt::ApplicationShortcut);

  m_testGame = new QAction(tr("Test game"), this);
  m_exportGame = new QAction(tr("Export game"), this);

  m_testGame->setShortcut(QKeySequence("CTRL+Enter"));
  m_testGame->setShortcutContext(Qt::ApplicationShortcut);
  /*
    m_tools = new QActionGroup{this};
    m_tools->addAction(m_moveAction);
    m_tools->addAction(m_resizeAction);
    m_tools->addAction(m_arrowAction);
    m_tools->setExclusive(true);

    m_moveAction->setShortcut(Qt::Key_Q);
    m_moveAction->setCheckable(true);
    m_resizeAction->setShortcut(Qt::Key_S);
    m_resizeAction->setCheckable(true);
    m_arrowAction->setShortcut(Qt::Key_D);
    m_arrowAction->setCheckable(true);

    connect(m_moveAction, &QAction::triggered,
            this, [=] {
    });
    connect(m_resizeAction, &QAction::triggered,
            this, [=] {
    });
    connect(m_arrowAction, &QAction::triggered,
            this, [=] {
    });
*/
  connect(
        m_copy_act, &QAction::triggered, this, &ApplicationPlugin::on_copy);
  connect(
        m_paste_act, &QAction::triggered, this, &ApplicationPlugin::on_paste);
  connect(
        m_help_act, &QAction::triggered, this, &ApplicationPlugin::on_help);
  connect(
        m_delete_act,
        &QAction::triggered,
        this,
        &ApplicationPlugin::on_delete);
  connect(
        m_stop_sound, &QAction::triggered, this, &ApplicationPlugin::on_stop);
  connect(
        m_movefront,
        &QAction::triggered,
        this,
        &ApplicationPlugin::on_moveForward);
  connect(
        m_moveback,
        &QAction::triggered,
        this,
        &ApplicationPlugin::on_moveBackwards);
  connect(
        m_zoomplus, &QAction::triggered, this, &ApplicationPlugin::on_zoom);
  connect(
        m_zoomminus, &QAction::triggered, this, &ApplicationPlugin::on_dezoom);
  connect(
        m_recenter, &QAction::triggered, this, [this] {
    auto doc = currentDocument();
    if(!doc)
      return;
    on_recenter(*doc);
  });


  connect(m_testGame, &QAction::triggered, this, &ApplicationPlugin::on_testGame);
  connect(m_exportGame, &QAction::triggered, this, &ApplicationPlugin::on_exportGame);

  m_help = new QDialog;
  auto lay = new QHBoxLayout{m_help};

  auto doc = qApp->applicationDirPath() + "/Documentation/SEGMentEditor.qhc";
  auto w = new HelpWidget{doc, m_help};
  lay->addWidget(w);
}

void ApplicationPlugin::on_createdDocument(score::Document& doc)
{
  // m_moveAction->setChecked(true);

  auto w = doc.view()->viewDelegate().getWidget();
  w->addAction(m_copy_act);
  w->addAction(m_paste_act);
  w->addAction(m_delete_act);

  on_recenter(doc);
}

void ApplicationPlugin::on_documentChanged(score::Document* olddoc, score::Document* newdoc)
{

}

void ApplicationPlugin::on_recenter(score::Document& doc)
{
  auto& m = static_cast<const SEGMent::DocumentModel&>(doc.model().modelDelegate());
  auto& v = static_cast<const SEGMent::DocumentView&>(doc.view()->viewDelegate());

  if(m.process().scenes.size() > 0)
  {
    double x0{std::numeric_limits<double>::max()}, y0{x0}, x1{std::numeric_limits<double>::lowest()}, y1{x1};

    for(SceneModel& scene : m.process().scenes)
    {
      const auto r = scene.rect();
      if(x0 > r.x()) x0 = r.x();
      if(y0 > r.y()) y0 = r.y();
      if(x1 < r.x() + r.width()) x1 = r.x() + r.width();
      if(y1 < r.y() + r.height()) y1 = r.y() + r.height();
    }

    x0 -= (0.1 * (x1 - x0));
    y0 -= (0.1 * (x1 - x0));
    const double w = 1.1 * (x1 - x0);
    const double h = 1.1 * (y1 - y0);

    v.view().fitInView(x0, y0, w, h, Qt::KeepAspectRatio);
    v.view().updateZoom();
  }

}

void ApplicationPlugin::on_testGame()
{
  score::Document* doc = currentDocument();
  if (!doc)
    return;

  // Save into a temp file
  auto segment_file = doc->metadata().fileName();
  QFileInfo fi{segment_file};
  auto path = fi.absolutePath() + "/__gametest__" + fi.fileName();
  qDebug() << path;

  QSaveFile f{path};
  f.open(QIODevice::WriteOnly);
  f.write(QJsonDocument{doc->saveAsJson()}.toJson());
  f.commit();

  auto segment_path = applicationPath();
  qDebug() << "Dir path:  " << segment_path;

  auto process = new QProcess;
#if defined(__linux__)
  process->setProgram(segment_path + "/engine/Linux/segment.x86_64");
#elif defined(_WIN32)
  process->setProgram(segment_path + "/engine/Windows/segment.exe");
#elif defined(__APPLE__)
  process->setProgram(segment_path + "/engine/macOS/segment.app");
#else
  qDebug(" Unknown OS ! ");
  return;
#endif


#if defined(_WIN32)
  QString game_folder = QFileInfo{path}.absolutePath() + "/";
  QString game_file = QFileInfo{path}.absoluteFilePath();

  game_folder.replace("/", "\\");
  game_file.replace("/", "\\");
#else
  QString game_folder = "file://" + QFileInfo{path}.absolutePath() + "/";
  QString game_file = "file://" + QFileInfo{path}.absoluteFilePath();
#endif

  // The environment is used to pass the file url to the segment game engine
  QProcessEnvironment env = QProcessEnvironment::systemEnvironment();

  env.insert("SEGMENT_GAME_FOLDER", game_folder);
  env.insert("SEGMENT_GAME_FILE", game_file);

  process->setProcessEnvironment(env);
  process->start();
  process->waitForFinished();

  // Delete the temp file
  QFile::remove(path);
}

void ApplicationPlugin::on_exportGame()
{
  score::Document* doc = currentDocument();
  if (!doc)
    return;

  QDir dir = QFileDialog::getExistingDirectory(this->context.mainWindow, tr("Select where to save the game"));
  if(!dir.exists())
    return;

  // First copy the executables
  auto segment_path = applicationPath();
  copyRecursively(segment_path + "/engine/Linux", dir.path() + "/Linux");
  copyRecursively(segment_path + "/engine/Windows", dir.path() + "/Windows");
  copyRecursively(segment_path + "/engine/macOS", dir.path() + "/macOS");

  // Then copy the games
  const auto target_paths = {dir.path() + "/Linux/game",
                      dir.path() + "/Windows/game",
                      dir.path() + "/macOS/game"};


  auto segment_file = doc->metadata().fileName();
  {
    QSaveFile f{segment_file};
    f.open(QIODevice::WriteOnly);
    f.write(QJsonDocument{doc->saveAsJson()}.toJson());
    f.commit();
  }

  auto& m = static_cast<const SEGMent::DocumentModel&>(doc->model().modelDelegate());

  const QFileInfo fi{segment_file};
  auto folder_path = fi.absolutePath();
  QFile f{segment_file};

  for(auto target_game : target_paths)
  {
      QDir{}.mkpath(target_game + "/Objects");
      QDir{}.mkpath(target_game + "/Scenes");
      QDir{}.mkpath(target_game + "/Sounds");
      QDir{}.mkpath(target_game + "/Templates");

      copyRecursively(folder_path + "/Diary", target_game + "/Diary");
      copyRecursively(folder_path + "/UI", target_game + "/UI");
      copyRecursively(folder_path + "/Videos", target_game + "/Videos");
  }

  ExportVisitor vis{[&] (const QString& path) {
          QFileInfo fi{path};

          if(fi.isAbsolute())
          {
              for(auto target_game : target_paths)
              {
                  QFile::copy(path, target_game + "/" + path);
              }
          }
          else
          {
              for(auto target_game : target_paths)
              {
                  QFile::copy(folder_path + "/" + path, target_game + "/" + path);
              }
          }
      }
  };
  for(auto& scene : m.process().scenes)
      vis(scene);
  for(auto& trans : m.process().transitions)
      vis(trans);

  for(auto target_game : target_paths)
  {
    f.copy(target_game + "/Game.segment");
  }

  QDesktopServices::openUrl(dir.path());
}

score::GUIApplicationPlugin::GUIElements ApplicationPlugin::makeGUIElements()
{
  auto view = score::GUIAppContext().mainWindow;
  view->addAction(m_help_act);
  view->addAction(m_movefront);
  view->addAction(m_moveback);
  view->addAction(m_zoomplus);
  view->addAction(m_zoomminus);
  view->addAction(m_recenter);
  view->addAction(m_testGame);

  /*
          view->addAction(m_moveAction);
          view->addAction(m_resizeAction);
          view->addAction(m_arrowAction);
          m_tools->setParent(view);
    */
  GUIElements e;

  auto& toolbars = e.toolbars;
  {
    auto bar = new QToolBar;
    bar->addAction(m_copy_act);
    bar->addAction(m_paste_act);
    bar->addAction(m_delete_act);
    bar->addAction(m_stop_sound);

    bar->addSeparator();

    bar->addAction(m_movefront);
    bar->addAction(m_moveback);

    bar->addSeparator();

    bar->addAction(m_zoomplus);
    bar->addAction(m_zoomminus);
    bar->addAction(m_recenter);

    bar->addSeparator();
    bar->addAction(m_testGame);
    bar->addAction(m_exportGame);
    bar->addSeparator();

    bar->addAction(m_help_act);
    toolbars.emplace_back(
          bar, StringKey<score::Toolbar>("Edit"), Qt::TopToolBarArea, 500);
  }
  /*
          {
            auto bar = new QToolBar;
            bar->addAction(m_moveAction);
            bar->addAction(m_resizeAction);
            bar->addAction(m_arrowAction);
            toolbars.emplace_back(bar, StringKey<score::Toolbar>("Action"),
       Qt::TopToolBarArea, 550);
          }
    */

  auto& edit = context.menus.get().at(score::Menus::Edit());
  edit.menu()->addAction(m_copy_act);
  edit.menu()->addAction(m_paste_act);
  edit.menu()->addAction(m_delete_act);
  auto& h = context.menus.get().at(score::Menus::About());
  h.menu()->addAction(m_help_act);
  return e;
}

struct SharedTransitionVisitor
{
  std::unordered_set<Path<SceneModel>> scenes;
  bool operator()(const SceneToScene& t) const noexcept
  {
    return scenes.find(t.from) != scenes.end()
        && scenes.find(t.to) != scenes.end();
  }
  template<typename T>
  bool operator()(const T& t) const noexcept
  {
    return scenes.find(t.from.template splitLast<SceneModel>().first) != scenes.end()
        && scenes.find(t.to) != scenes.end();
  }
};

/**
 * @brief Finds all the transitions which are contained in a selection.
 *
 * e.g. if we select two scenes, and an object of the first scene
 * has a transition to the second scene, we want to select that transition.
 */
std::vector<TransitionModel*> sharedTransitions(const Selection& sel, const SEGMent::ProcessModel& process)
{
  std::vector<TransitionModel*> res;

  SharedTransitionVisitor vis;
  std::unordered_set<Path<SceneModel>>& scenes = vis.scenes;
  for(auto obj : sel)
  {
    dispatch(obj.data(), [&] (auto& t) {
      using obj_t = std::remove_const_t<std::remove_reference_t<decltype(t)>>;
      if constexpr(std::is_same_v<obj_t, SceneModel>) {
        scenes.insert(t);
      }
    });
  }

  for(auto& trans : process.transitions)
  {
    if(eggs::variants::apply(vis, trans.transition()))
    {
      res.push_back(&trans);
    }
  }

  return res;
}

void ApplicationPlugin::on_copy()
{
  score::Document* doc = currentDocument();
  if (!doc)
    return;

  const auto& sel = doc->context().selectionStack.currentSelection();
  CopyObjectVisitor vis;
  for (auto e : sel)
  {
    dispatch(e.data(), vis);
  }

  QJsonObject obj;
  obj["DocumentPath"] = doc->metadata().fileName();

  if (vis.scenes.empty())
  {
    if(!vis.objects.empty())
    {
      auto d = new QMimeData;
      obj["Objects"] = vis.objects;
      obj["Resources"] = vis.resources.resources;

      d->setData("segment/objects", QJsonDocument{obj}.toBinaryData());
      QApplication::clipboard()->setMimeData(d, QClipboard::Clipboard);
    }
  }
  else
  {
    auto& process = static_cast<SEGMent::DocumentModel&>(doc->model().modelDelegate()).process();
    auto d = new QMimeData;

    auto transitions = sharedTransitions(sel, process);
    QJsonArray arr;
    for(auto t : transitions)
    {
      arr.push_back(score::marshall<JSONObject>(*t));
      if(const auto& sound = t->sound().path(); !sound.isEmpty())
        vis.resources.resources.push_back(t->sound().path());
    }
    obj["Transitions"] = arr;
    obj["Scenes"] = vis.scenes;
    obj["Resources"] = vis.resources.resources;

    d->setData("segment/scenes", QJsonDocument{obj}.toBinaryData());
    QApplication::clipboard()->setMimeData(d, QClipboard::Clipboard);
  }
}

void ApplicationPlugin::on_paste()
{
  score::Document* doc = currentDocument();
  if (!doc)
    return;
  DocumentView& view
      = static_cast<DocumentView&>(doc->view()->viewDelegate());

  QPoint widget_pos = view.graphicsView().mapFromGlobal(QCursor::pos());
  QPointF scene_pos = view.graphicsView().mapToScene(widget_pos);

  // auto scene
  auto m = QApplication::clipboard()->mimeData();
  if (!m)
    return;

  ObjectPaster paster{*doc, view, widget_pos, scene_pos};
  paster.paste(m);
}

void ApplicationPlugin::on_zoom()
{
  score::Document* doc = currentDocument();
  if (!doc)
    return;
  DocumentView& view
      = static_cast<DocumentView&>(doc->view()->viewDelegate());

  view.graphicsView().zoom(100);
}

void ApplicationPlugin::on_dezoom()
{
  score::Document* doc = currentDocument();
  if (!doc)
    return;
  DocumentView& view
      = static_cast<DocumentView&>(doc->view()->viewDelegate());

  view.graphicsView().zoom(-100);
}

void ApplicationPlugin::on_help()
{
  m_help->show();
}

void ApplicationPlugin::on_delete()
{
  score::Document* doc = currentDocument();
  if (!doc)
    return;

  MacroCommandDispatcher<RemoveObjects> disp{doc->context().commandStack};
  const auto& sel = doc->context().selectionStack.currentSelection();

  RemoveObjectVisitor vis{disp};
  for (const auto& e : sel)
  {
    dispatch(e.data(), vis);
  }
  vis.finish();

  if(disp.command()->count() != 0)
      disp.commit();
}

void ApplicationPlugin::on_stop()
{
  SoundPlayer::instance().stop();
}


void ApplicationPlugin::on_moveForward()
{
  score::Document* doc = currentDocument();
  if (!doc)
    return;

  const auto& sel = doc->context().selectionStack.currentSelection();
  for (auto e : sel)
  {
    dispatch(e.data(), MoveForwardVisitor{doc->context()});
  }
}

void ApplicationPlugin::on_moveBackwards()
{
  score::Document* doc = currentDocument();
  if (!doc)
    return;

  const auto& sel = doc->context().selectionStack.currentSelection();
  for (auto e : sel)
  {
    dispatch(e.data(), MoveBackwardsVisitor{doc->context()});
  }
}

}


