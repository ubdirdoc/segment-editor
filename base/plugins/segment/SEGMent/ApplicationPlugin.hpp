#pragma once
#include <score/plugins/application/GUIApplicationPlugin.hpp>

namespace SEGMent
{
class ApplicationPlugin
    : public QObject
    , public score::GUIApplicationPlugin
{
public:
  ApplicationPlugin(const score::GUIApplicationContext& presenter);

private:
  void on_createdDocument(score::Document& doc) override;
  void on_documentChanged(score::Document* olddoc, score::Document* newdoc) override;


  GUIElements makeGUIElements() override;

  void on_copy();
  void on_paste();

  void on_moveForward();
  void on_moveBackwards();

  void on_zoom();
  void on_dezoom();

  void on_help();
  void on_delete();
  void on_stop();

  void on_recenter(score::Document& doc);

  void on_testGame();
  void on_exportGame();

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
  QAction* m_recenter{};

  QAction* m_testGame{};
  QAction* m_exportGame{};

  /*
  QAction* m_moveAction{};
  QAction* m_resizeAction{};
  QAction* m_arrowAction{};
  QActionGroup* m_tools{};
  */
};
} // namespace SEGMent
