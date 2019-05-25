#pragma once
#include <score/plugins/panel/PanelDelegate.hpp>
#include <score/plugins/panel/PanelDelegateFactory.hpp>

#include <QGraphicsScene>
#include <QGraphicsView>

namespace SEGMent
{

class Minimap final
    : public QObject
    , public score::PanelDelegate
{
    W_OBJECT(Minimap)
public:
  Minimap(const score::GUIApplicationContext& ctx);

private:
  QWidget* widget() override;
  const score::PanelStatus& defaultPanelStatus() const override;
  void on_modelChanged(score::MaybeDocument oldm, score::MaybeDocument newm) override;

  QWidget* m_widget{};
  QGraphicsScene* m_scene{};
  QGraphicsView* m_view{};
};

class MinimapFactory final : public score::PanelDelegateFactory
{
  SCORE_CONCRETE("105f5a98-4012-449c-8942-59ac4929d2b6")

  std::unique_ptr<score::PanelDelegate>
  make(const score::GUIApplicationContext& ctx) override;
};

}
