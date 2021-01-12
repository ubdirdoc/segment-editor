#pragma once
#include <score/plugins/panel/PanelDelegate.hpp>
#include <score/plugins/panel/PanelDelegateFactory.hpp>

#include <QTabWidget>

#include <SEGMent/Browser/ImageList.hpp>
#include <SEGMent/Browser/SoundList.hpp>
#include <SEGMent/Browser/TemplateList.hpp>
namespace SEGMent
{
class ImageList;

//! Library panel of SEGMent
class Library final : public QObject, public score::PanelDelegate
{
  W_OBJECT(Library)
public:
  Library(const score::GUIApplicationContext& ctx);

private:
  QWidget* widget() override;

  const score::PanelStatus& defaultPanelStatus() const override;

  void on_modelChanged(score::MaybeDocument oldm, score::MaybeDocument newm)
      override;
  void on_documentPathChanged(const QString& p);
  void setNewSelection(const Selection& s) override;

  ImageList* m_scenes{};
  ImageList* m_objects{};
  TemplateList* m_templates{};
  QWidget* m_zones{};
  SoundList* m_sounds{};
  QTabWidget* m_widget{};

  void reloadImageList();
};

class LibraryFactory final : public score::PanelDelegateFactory
{
  SCORE_CONCRETE("83F5B2F8-0008-4AFC-9C51-E35900582B78")

  std::unique_ptr<score::PanelDelegate>
  make(const score::GUIApplicationContext& ctx) override;
};

} // namespace SEGMent
