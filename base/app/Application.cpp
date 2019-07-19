// This is an open source non-commercial project. Dear PVS-Studio, please check
// it. PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "Application.hpp"

#include <score/application/ApplicationComponents.hpp>
#include <score/application/GUIApplicationContext.hpp>
#include <score/command/Validity/ValidityChecker.hpp>
#include <score/model/Identifier.hpp>
#include <score/model/path/ObjectIdentifier.hpp>
#include <score/plugins/application/GUIApplicationPlugin.hpp>
#include <score/plugins/documentdelegate/DocumentDelegateFactory.hpp>
#include <score/selection/Selection.hpp>
#include <score/tools/IdentifierGeneration.hpp>
#include <score/tools/std/Optional.hpp>
#include <score/widgets/Pixmap.hpp>

#include <core/application/ApplicationRegistrar.hpp>
#include <core/application/ApplicationSettings.hpp>
#include <core/application/SafeQApplication.hpp>
#include <core/document/DocumentBackups.hpp>
#include <core/document/DocumentModel.hpp>
#include <core/presenter/DocumentManager.hpp>
#include <core/presenter/Presenter.hpp>
#include <core/view/Window.hpp>

#include <QByteArray>
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFont>
#include <QFontDatabase>
#include <QIODevice>
#include <QMetaType>
#include <QPainter>
#include <QPixmap>
#include <QScreen>
#include <QSplashScreen>
#include <QString>
#include <QStringList>
#include <QStyle>
#include <QStyleFactory>
#include <qnamespace.h>

#include <algorithm>
#include <vector>

#if __has_include(<QQuickStyle>)
#include <QQuickStyle>
#endif

#if defined(SCORE_STATIC_PLUGINS)
#include <score_static_plugins.hpp>
#endif

#include <wobjectimpl.h>
W_OBJECT_IMPL(Application)

#if defined(SCORE_STATIC_PLUGINS)
int qInitResources_resources();
#endif

namespace score
{
class DocumentModel;

static void setQApplicationSettings(QApplication& m_app)
{
#if defined(SCORE_STATIC_PLUGINS)
  qInitResources_resources();
#endif
}

} // namespace score

Application::Application(int& argc, char** argv) : QObject{nullptr}
{
  m_instance = this;

  QStringList l;
  for (int i = 0; i < argc; i++)
    l.append(QString::fromUtf8(argv[i]));
  m_applicationSettings.parse(l, argc, argv);

  if (m_applicationSettings.gui)
    m_app = new SafeQApplication{argc, argv};
  else
    m_app = new QCoreApplication{argc, argv};
}

Application::Application(
    const score::ApplicationSettings& appSettings,
    int& argc,
    char** argv)
    : QObject{nullptr}, m_applicationSettings(appSettings)
{
  m_instance = this;
  if (m_applicationSettings.gui)
    m_app = new SafeQApplication{argc, argv};
  else
    m_app = new QCoreApplication{argc, argv};
}

Application::~Application()
{
  this->setParent(nullptr);
  delete m_view;
  delete m_presenter;

  score::DocumentBackups::clear();
  QCoreApplication::processEvents();
  delete m_app;
}

const score::GUIApplicationContext& Application::context() const
{
  return m_presenter->applicationContext();
}

const score::ApplicationComponents& Application::components() const
{
  return m_presenter->applicationComponents();
}

void Application::init()
{
#if defined(SCORE_STATIC_PLUGINS)
  score_init_static_plugins();
#endif

  this->setObjectName("Application");
  this->setParent(m_app);
  m_app->addLibraryPath(m_app->applicationDirPath() + "/plugins");

  // MVP
  if (m_applicationSettings.gui)
  {
    score::setQApplicationSettings(*qApp);
    m_settings.setupView();
    m_projectSettings.setupView();
    m_view = new score::View{this};
  }

  m_presenter = new score::Presenter{
      m_applicationSettings, m_settings, m_projectSettings, m_view, this};

  // Plugins
  loadPluginData();

  // View
  if (m_applicationSettings.gui)
  {
    m_view->show();
  }

  initDocuments();

  if (m_applicationSettings.gui)
  {
    m_view->sizeChanged(m_view->size());
    m_view->ready();
  }
}

void Application::initDocuments()
{
  auto& ctx = m_presenter->applicationContext();
  if (!m_applicationSettings.loadList.empty())
  {
    for (const auto& doc : m_applicationSettings.loadList)
      m_presenter->documentManager().loadFile(ctx, doc);
  }

  // The plug-ins have the ability to override the boot process.
  for (auto plug : ctx.guiApplicationPlugins())
  {
    if (plug->handleStartup())
    {
      return;
    }
  }

  if (auto sqa = dynamic_cast<SafeQApplication*>(m_app))
  {
    connect(
        sqa, &SafeQApplication::fileOpened, this, [&](const QString& file) {
          m_presenter->documentManager().loadFile(ctx, file);
        });
  }

  // Try to reload if there was a crash
  if (m_applicationSettings.tryToRestore
      && score::DocumentBackups::canRestoreDocuments())
  {
    m_presenter->documentManager().restoreDocuments(ctx);
  }

  // If nothing was reloaded, open a normal document
  if (m_presenter->documentManager().documents().empty())
  {
    auto& documentKinds = m_presenter->applicationComponents()
                              .interfaces<score::DocumentDelegateList>();
    if (!documentKinds.empty()
        && m_presenter->documentManager().documents().empty())
    {
      m_presenter->documentManager().newDocument(
          ctx,
          Id<score::DocumentModel>{score::random_id_generator::getRandomId()},
          *m_presenter->applicationComponents()
               .interfaces<score::DocumentDelegateList>()
               .begin());
    }
  }
}

void Application::loadPluginData()
{
  auto& ctx = m_presenter->applicationContext();
  score::GUIApplicationRegistrar registrar{m_presenter->components(),
                                           ctx,
                                           m_presenter->menuManager(),
                                           m_presenter->toolbarManager(),
                                           m_presenter->actionManager()};

  GUIApplicationInterface::loadPluginData(
      ctx, registrar, m_settings, *m_presenter);
}

int Application::exec()
{
  return m_app->exec();
}
