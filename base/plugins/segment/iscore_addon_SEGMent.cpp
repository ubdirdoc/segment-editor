#include "iscore_addon_SEGMent.hpp"

#include <score/plugins/customfactory/FactorySetup.hpp>

#include <SEGMent/ApplicationPlugin.hpp>
#include <SEGMent/Commands/CommandFactory.hpp>
#include <SEGMent/Commands/Creation.hpp>
#include <SEGMent/Commands/Deletion.hpp>
#include <SEGMent/Commands/Properties.hpp>
#include <SEGMent/Document.hpp>
#include <SEGMent/Inspector/Inspectors.hpp>
#include <SEGMent/Items/GlobalVariables.hpp>
#include <SEGMent/Model/Layer/ProcessPresenter.hpp>
#include <SEGMent/Model/Layer/ProcessView.hpp>
#include <SEGMent/Panel/Library.hpp>
#include <QTranslator>
#include <iscore_addon_SEGMent_commands_files.hpp>
#include <wobjectimpl.h>
W_OBJECT_IMPL(SEGMent::EditingFinishedTextEdit)
iscore_addon_SEGMent::iscore_addon_SEGMent()
{
#if defined(SCORE_STATIC_PLUGINS)
  Q_INIT_RESOURCE(segment);
#endif
  auto tra = new QTranslator;
  if (tra->load(":/translations/fr.qm"))
  {
    qApp->installTranslator(tra);
  }
}

iscore_addon_SEGMent::~iscore_addon_SEGMent() {}

std::vector<std::unique_ptr<score::InterfaceBase>>
iscore_addon_SEGMent::factories(
    const score::ApplicationContext& ctx,
    const score::InterfaceKey& key) const
{
  return instantiate_factories<
      score::ApplicationContext,
      FW<score::DocumentDelegateFactory, SEGMent::DocumentFactory>,
      FW<Inspector::InspectorWidgetFactory,
         SEGMent::SceneInspectorFactory,
         SEGMent::ImageInspectorFactory,
         SEGMent::GifInspectorFactory,
         SEGMent::TransitionInspectorFactory,
         SEGMent::TextInspectorFactory,
         SEGMent::ClickInspectorFactory,
         SEGMent::BackClickInspectorFactory>,
      FW<score::PanelDelegateFactory, SEGMent::LibraryFactory>>(ctx, key);
}

std::pair<const CommandGroupKey, CommandGeneratorMap>
iscore_addon_SEGMent::make_commands()
{
  using namespace SEGMent;
  std::pair<const CommandGroupKey, CommandGeneratorMap> cmds{
      CommandFactoryName(), CommandGeneratorMap{}};

  // CMake generates the "iscore_addon_SEGMent_commands.hpp" and
  // "iscore_addon_SEGMent_commands_files.hpp" by scanning the source files for
  // \ref SCORE_COMMAND_DECL or \ref SCORE_COMMAND_DECL_T.

  ossia::for_each_type<
#include <QTranslator>
#include <iscore_addon_SEGMent_commands.hpp>
      >(score::commands::FactoryInserter{cmds.second});

  return cmds;
}

score::GUIApplicationPlugin* iscore_addon_SEGMent::make_guiApplicationPlugin(
    const score::GUIApplicationContext& app)
{
  return new SEGMent::ApplicationPlugin{app};
}
