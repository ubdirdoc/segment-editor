#pragma once
#include <score/application/ApplicationContext.hpp>
#include <score/command/Command.hpp>
#include <score/command/CommandGeneratorMap.hpp>
#include <score/plugins/customfactory/FactoryFamily.hpp>
#include <score/plugins/qt_interfaces/CommandFactory_QtInterface.hpp>
#include <score/plugins/qt_interfaces/FactoryFamily_QtInterface.hpp>
#include <score/plugins/qt_interfaces/FactoryInterface_QtInterface.hpp>
#include <score/plugins/qt_interfaces/GUIApplicationPlugin_QtInterface.hpp>
#include <score/plugins/qt_interfaces/PluginRequirements_QtInterface.hpp>

#include <QObject>

#include <utility>
#include <vector>

/**
 * \namespace SEGMent
 *
 * This is a GUI editor for SEGMent games, implemented as an ossia score
 * plug-in. For more information on ossia score, refer to https://ossia.io.
 *
 * This plug-in provides a new document kind : a SEGMent document.
 * This document represents an infinite visual canvas on which 2D scenes can be
 * positioned.
 *
 * The code organization is as follows :
 *
 * * The Commands folder contains undo-redo commands.
 * * The Inspector folder contains the inspector widgets for all elements.
 * * The Items folder contains the visual items.
 * * The Model folder contains the data model.
 * * The Panel folder contains the image library panel.
 *
 */

class iscore_addon_SEGMent final : public score::Plugin_QtInterface,
                                   public score::FactoryInterface_QtInterface,
                                   public score::CommandFactory_QtInterface,
                                   public score::ApplicationPlugin_QtInterface
{
  // Version, and unique identifier for the plug-in.
  // The uuid can be generated with "uuidgen" on linux or OS X.
  SCORE_PLUGIN_METADATA(1, "FCBC56CD-4504-41C0-BC7E-C1D7426C39CC")

public:
  iscore_addon_SEGMent();
  ~iscore_addon_SEGMent() override;

private:
  std::vector<std::unique_ptr<score::InterfaceBase>> factories(
      const score::ApplicationContext& ctx,
      const score::InterfaceKey& key) const override;

  std::pair<const CommandGroupKey, CommandGeneratorMap>
  make_commands() override;

  score::GUIApplicationPlugin*
  make_guiApplicationPlugin(const score::GUIApplicationContext& app) override;
};
