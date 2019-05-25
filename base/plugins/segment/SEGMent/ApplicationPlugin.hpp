#pragma once
#include <SEGMent/Model/Scene.hpp>
#include <SEGMent/Model/Layer/ProcessView.hpp>
#include <SEGMent/Items/ObjectWindow.hpp>
#include <SEGMent/Items/ClickWindow.hpp>
#include <SEGMent/Items/SceneWindow.hpp>
#include <SEGMent/Commands/Creation.hpp>
#include <SEGMent/Commands/Deletion.hpp>
#include <SEGMent/Document.hpp>
#include <core/document/DocumentView.hpp>
#include <score/plugins/documentdelegate/DocumentDelegateView.hpp>
#include <score/plugins/application/GUIApplicationPlugin.hpp>
#include <score/actions/Menu.hpp>
#include <score/actions/MenuManager.hpp>
#include <score/command/Dispatchers/CommandDispatcher.hpp>
#include <score/document/DocumentContext.hpp>
#include <score/document/DocumentInterface.hpp>
#include <core/document/Document.hpp>
#include <core/document/DocumentModel.hpp>
#include <QGraphicsView>
#include <score/command/Dispatchers/CommandDispatcher.hpp>
#include <score/command/Dispatchers/MacroCommandDispatcher.hpp>
#include <wobjectimpl.h>
#include <QTranslator>
#include <QGraphicsView>
#include <QJsonDocument>
#include <QClipboard>
#include <QSplitter>
#include <QMainWindow>
#include <QTextBrowser>
#include <QMessageBox>
#include <QHelpEngine>
#include <QHelpIndexWidget>
#include <QHelpContentWidget>
#include <QTabWidget>
#include <QMimeData>
#include <QApplication>
#include <QDialog>
#include <QFileInfo>
#include <QHBoxLayout>
#include <typeindex>
#include <cmath>
#include <SEGMent/Items/SceneWindow.hpp>
#include <score_git_info.hpp>
namespace SEGMent
{

class HelpBrowser final
    : public QTextBrowser
{
  public:
    HelpBrowser(QHelpEngine* helpEngine, QWidget* parent = 0)
      : QTextBrowser{parent}
      , m_help_engine{helpEngine}
    {
      setMinimumWidth(1024);
      setMinimumHeight(768);
    }

    QVariant loadResource(int type, const QUrl& name) override
    {
      if (name.scheme() == "qthelp")
        return QVariant(m_help_engine->fileData(name));
      else
        return QTextBrowser::loadResource(type, name);
    }

  private:
    QHelpEngine* m_help_engine{};
};

class HelpWidget : public QSplitter
{
  public:
    HelpWidget(QString help_path, QWidget *parent = nullptr) :
      QSplitter{Qt::Horizontal, parent},
      m_help_engine{help_path, this},
      m_help_browser{&m_help_engine, this}
    {
      auto content = m_help_engine.contentWidget();
      content->setMinimumWidth(300);
      content->setMaximumWidth(300);

      m_help_browser.setSource(QUrl("qthelp://org.sphinx.segmenteditor." + QString::number(SCORE_VERSION_MAJOR) + "." + QString::number(SCORE_VERSION_MINOR) + "/doc/index.html"));
      connect(m_help_engine.contentWidget(), &QHelpContentWidget::linkActivated,
              &m_help_browser, &HelpBrowser::setSource);
      connect(m_help_engine.indexWidget(), &QHelpIndexWidget::linkActivated,
              &m_help_browser, &HelpBrowser::setSource);

      insertWidget(0, content);
      insertWidget(1, &m_help_browser);
    }

  private:
    QHelpEngine m_help_engine;
    HelpBrowser m_help_browser;
};

template<typename F>
auto dispatch(const QObject* obj, F&& fun)
{
  if(auto v = qobject_cast<const SceneModel*>(obj))
    return fun(*v);
  if(auto v = qobject_cast<const ImageModel*>(obj))
    return fun(*v);
  if(auto v = qobject_cast<const GifModel*>(obj))
    return fun(*v);
  if(auto v = qobject_cast<const ClickAreaModel*>(obj))
    return fun(*v);
  if(auto v = qobject_cast<const BackClickAreaModel*>(obj))
    return fun(*v);
  if(auto v = qobject_cast<const TextAreaModel*>(obj))
    return fun(*v);
  if(auto v = qobject_cast<const TransitionModel*>(obj))
    return fun(*v);

  if constexpr(!std::is_same_v<decltype(fun(std::declval<const SceneModel&>())), void>)
    return decltype(fun(std::declval<const SceneModel&>())){};
}


template<typename F>
auto dispatchItem(const QGraphicsItem* obj, F&& fun)
{
  if(auto v = dynamic_cast<const SceneWindow*>(obj))
    return fun(v->model());
  if(auto v = dynamic_cast<const ImageWindow*>(obj))
    return fun(v->model());
  if(auto v = dynamic_cast<const GifWindow*>(obj))
    return fun(v->model());
  if(auto v = dynamic_cast<const ClickWindow*>(obj))
    return fun(v->model());
  if(auto v = dynamic_cast<const BackClickWindow*>(obj))
    return fun(v->model());
  if(auto v = dynamic_cast<const TextWindow*>(obj))
    return fun(v->model());

  return decltype(fun(std::declval<const SceneModel&>())){};
}

struct CopyObjectVisitor
{
    QMimeData* operator()(const SceneModel& obj) const
    {
      auto json = score::marshall<JSONObject>(obj);

      auto d = new QMimeData;
      d->setData("segment/scene", QJsonDocument{json}.toBinaryData());
      return d;
    }

    QMimeData* operator()(const TransitionModel& obj) const
    {
      return nullptr;
    }

    template<typename T>
    QMimeData* operator()(const T& obj) const
    {
      auto json = score::marshall<JSONObject>(obj);
      json["ObjectKind"] = Metadata<ObjectKey_k, T>::get();


      auto d = new QMimeData;
      d->setData("segment/object", QJsonDocument{json}.toBinaryData());
      return d;
    }
};

class SceneZOrders
{
public:
  template<typename T>
  SceneZOrders(const SceneModel& s, const T& self)
  {
    ossia::for_each_in_tuple(
          std::tie(s.objects, s.gifs, s.clickAreas, s.backClickAreas, s.textAreas)
          , [&] (const auto& map){
      for(auto& obj : map)
      {
        using obj_t = std::remove_reference_t<std::remove_const_t<decltype(obj)>>;
        if constexpr(std::is_same_v<T, obj_t>)
        {
          if(&obj == &self)
            continue;
        }
        min = std::min(min, obj.z());
        max = std::max(max, obj.z());
        count++;
      }
    });

  }
  int min{INT_MAX}, max{0};
  int count = 0;
};

struct MoveForwardVisitor
{
  const score::DocumentContext& context;
    void operator()(const SceneModel& obj) const
    {
    }

    void operator()(const TransitionModel& obj) const
    {
    }

    template<typename T>
    void operator()(const T& obj) const
    {
      using property = typename T::p_z;
      using cmd = typename score::PropertyCommand_T<property>::template command<void>::type;

      auto [min, max, count] = SceneZOrders{*safe_cast<SceneModel*>(obj.parent()), obj};
      if(count == 0)
        return;

      CommandDispatcher<>{context.commandStack}.submitCommand(new cmd{obj, std::min(obj.z() + 1, max + 1)});
    }
};

struct MoveBackwardsVisitor
{
  const score::DocumentContext& context;
    void operator()(const SceneModel& obj) const
    {
    }

    void operator()(const TransitionModel& obj) const
    {
    }

    template<typename T>
    void operator()(const T& obj) const
    {
      using property = typename T::p_z;
      using cmd = typename score::PropertyCommand_T<property>::template command<void>::type;

      auto [min, max, count] = SceneZOrders{*safe_cast<SceneModel*>(obj.parent()), obj};
      if(count == 0)
        return;
      if(min <= 1)
        min = 1;

      CommandDispatcher<>{context.commandStack}.submitCommand(new cmd{obj, std::max(obj.z() - 1, min - 1)});
    }
};
struct ApplicationPlugin
    : public QObject
    , public score::GUIApplicationPlugin
{
    ApplicationPlugin(const score::GUIApplicationContext& presenter)
      : GUIApplicationPlugin{presenter}
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

      m_zoomplus->setShortcut(QKeySequence::ZoomIn);
      m_zoomminus->setShortcut(QKeySequence::ZoomOut);
      m_zoomplus->setShortcutContext(Qt::ApplicationShortcut);
      m_zoomminus->setShortcutContext(Qt::ApplicationShortcut);
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
      connect(m_copy_act, &QAction::triggered,
              this, &ApplicationPlugin::on_copy);
      connect(m_paste_act, &QAction::triggered,
              this, &ApplicationPlugin::on_paste);
      connect(m_help_act, &QAction::triggered,
              this, &ApplicationPlugin::on_help);
      connect(m_delete_act, &QAction::triggered,
              this, &ApplicationPlugin::on_delete);
      connect(m_stop_sound, &QAction::triggered,
              this, &ApplicationPlugin::on_stop);
      connect(m_movefront, &QAction::triggered,
              this, &ApplicationPlugin::on_moveForward);
      connect(m_moveback, &QAction::triggered,
              this, &ApplicationPlugin::on_moveBackwards);
      connect(m_zoomplus, &QAction::triggered,
              this, &ApplicationPlugin::on_zoom);
      connect(m_zoomminus, &QAction::triggered,
              this, &ApplicationPlugin::on_dezoom);

      m_help = new QDialog;
      auto lay = new QHBoxLayout{m_help};

      auto doc = qApp->applicationDirPath() +"/Documentation/SEGMentEditor.qhc";
      auto w = new HelpWidget{doc, m_help};
      lay->addWidget(w);
    }

    void on_createdDocument(score::Document& doc) override
    {
      //m_moveAction->setChecked(true);

      auto w = doc.view()->viewDelegate().getWidget();
      w->addAction(m_copy_act);
      w->addAction(m_paste_act);
      w->addAction(m_delete_act);
    }

    GUIElements makeGUIElements() override
    {
      auto view = score::GUIAppContext().mainWindow;
      view->addAction(m_help_act);
      view->addAction(m_movefront);
      view->addAction(m_moveback);
      view->addAction(m_zoomplus);
      view->addAction(m_zoomminus);

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

        bar->addSeparator();

        bar->addAction(m_help_act);
        toolbars.emplace_back(bar, StringKey<score::Toolbar>("Edit"), Qt::TopToolBarArea, 500);
      }
/*
      {
        auto bar = new QToolBar;
        bar->addAction(m_moveAction);
        bar->addAction(m_resizeAction);
        bar->addAction(m_arrowAction);
        toolbars.emplace_back(bar, StringKey<score::Toolbar>("Action"), Qt::TopToolBarArea, 550);
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

    void on_copy()
    {
      score::Document* doc = currentDocument();
      if(!doc)
        return;

      const auto& sel = doc->context().selectionStack.currentSelection();
      for(auto e : sel)
      {
        if(auto d = dispatch(e.data(), CopyObjectVisitor{}))
        {
          QApplication::clipboard()->setMimeData(d, QClipboard::Clipboard);
        }
      }
    }

    void on_paste()
    {
      score::Document* doc = currentDocument();
      if(!doc)
        return;
      DocumentView& view = static_cast<DocumentView&>(doc->view()->viewDelegate());

      QPoint widget_pos = view.graphicsView().mapFromGlobal(QCursor::pos());
      QPointF scene_pos = view.graphicsView().mapToScene(widget_pos);

      //auto scene
      auto m = QApplication::clipboard()->mimeData();
      if(!m)
        return;

      if(m->hasFormat("segment/scene"))
      {
        auto json = QJsonDocument::fromBinaryData(m->data("segment/scene"));
        auto& model = score::IDocument::get<SEGMent::DocumentModel>(*doc);
        auto& proc = model.process();

        auto obj = json.object();
        auto rect = fromJsonValue<QRectF>(obj["Rect"]);
        auto pos = view.view().mapFromScene(scene_pos);

        obj["Rect"] = toJsonValue(QRectF{pos.x(), pos.y(), rect.width(), rect.height()});

        CommandDispatcher<> c{doc->context().commandStack};
        c.submitCommand(new PasteScene{proc, obj});
      }
      else if(m->hasFormat("segment/object"))
      {
        auto item_under_mouse = view.graphicsView().itemAt(widget_pos);
        if(!item_under_mouse)
          return;

        auto json = QJsonDocument::fromBinaryData(m->data("segment/object"));

        while(item_under_mouse->type() != SEGMent::SceneWindow::static_type())
        {
          item_under_mouse = item_under_mouse->parentItem();
          if(!item_under_mouse)
            return;
        }

        CommandDispatcher<> disp{doc->context().commandStack};
        auto& scene = static_cast<SceneWindow*>(item_under_mouse)->model();
        auto item_pos = item_under_mouse->mapFromScene(scene_pos);
        QPointF relative_item_pos = {
          item_pos.x() / item_under_mouse->boundingRect().width(),
          item_pos.y() / item_under_mouse->boundingRect().height(),
        };

        auto obj = json.object();
        obj["Pos"] = toJsonValue(relative_item_pos);
        if(obj["ObjectKind"] == Metadata<ObjectKey_k, ImageModel>::get())
        {
          disp.submitCommand(new PasteObject<ImageModel>(scene, obj));
        }
        else if(obj["ObjectKind"] == Metadata<ObjectKey_k, GifModel>::get())
        {
          disp.submitCommand(new PasteObject<GifModel>(scene, obj));
        }
        else if(obj["ObjectKind"] == Metadata<ObjectKey_k, ClickAreaModel>::get())
        {
          disp.submitCommand(new PasteObject<ClickAreaModel>(scene, obj));
        }
        else if(obj["ObjectKind"] == Metadata<ObjectKey_k, BackClickAreaModel>::get())
        {
          disp.submitCommand(new PasteObject<BackClickAreaModel>(scene, obj));
        }
        else if(obj["ObjectKind"] == Metadata<ObjectKey_k, TextAreaModel>::get())
        {
          disp.submitCommand(new PasteObject<TextAreaModel>(scene, obj));
        }
      }
    }

    void on_moveForward()
    {
      score::Document* doc = currentDocument();
      if(!doc)
        return;

      const auto& sel = doc->context().selectionStack.currentSelection();
      for(auto e : sel)
      {
        dispatch(e.data(), MoveForwardVisitor{doc->context()});
      }
    }


    void on_moveBackwards()
    {
      score::Document* doc = currentDocument();
      if(!doc)
        return;

      const auto& sel = doc->context().selectionStack.currentSelection();
      for(auto e : sel)
      {
        dispatch(e.data(), MoveBackwardsVisitor{doc->context()});
      }
    }


    void on_zoom()
    {
      score::Document* doc = currentDocument();
      if(!doc)
        return;
      DocumentView& view = static_cast<DocumentView&>(doc->view()->viewDelegate());

      view.graphicsView().zoom(100);
    }
    void on_dezoom()
    {
      score::Document* doc = currentDocument();
      if(!doc)
        return;
      DocumentView& view = static_cast<DocumentView&>(doc->view()->viewDelegate());

      view.graphicsView().zoom(-100);
    }

    void on_help()
    {
      m_help->show();
    }

    void on_delete()
    {
      score::Document* doc = currentDocument();
      if(!doc)
        return;

      MacroCommandDispatcher<RemoveObjects> disp{doc->context().commandStack};
      const auto& sel = doc->context().selectionStack.currentSelection();
      for(const auto& e : sel)
      {
        if(auto cmd = dispatch(e.data(), RemoveObjectVisitor{}))
          disp.submitCommand(cmd);
      }
      disp.commit();
    }

    void on_stop()
    {
      SoundPlayer::instance().stop();
    }

    QAction* m_copy_act{};
    QAction* m_paste_act{};
    QAction* m_help_act{};
    QAction* m_delete_act{};
    QDialog* m_help{};
    QAction* m_stop_sound{};

    QAction* m_movefront{};
    QAction* m_moveback{};

    QAction* m_zoomplus{};
    QAction* m_zoomminus{};

    /*
    QAction* m_moveAction{};
    QAction* m_resizeAction{};
    QAction* m_arrowAction{};
    QActionGroup* m_tools{};
    */
};
}
