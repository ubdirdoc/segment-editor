#pragma once
#include <SEGMent/Model/Cue.hpp>
#include <SEGMent/Commands/Properties.hpp>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QFrame>
#include <verdigris>
#include <array>
#include <QScrollArea>
#include <score/command/Dispatchers/CommandDispatcher.hpp>
namespace SEGMent
{
struct CueWidget : public QFrame {
  W_OBJECT(CueWidget)
  public:
    Cue m_cue;

  QHBoxLayout m_layout;

  QVBoxLayout m_keyLayout;
  QLineEdit m_cueName;
  QPushButton m_remove;

  QVBoxLayout m_editLayout;
  std::array<QLineEdit, 3> m_cueEdits;


  CueWidget(QWidget* parent = nullptr)
    : QFrame{parent}
    , m_layout{this}
  {
      setFrameStyle(QFrame::Box | QFrame::Plain);
      setLineWidth(1);
    while(m_cue.cues.size() < 3) m_cue.cues.push_back("");
    m_layout.setContentsMargins(0,0,0,0);
    m_layout.setSpacing(0);
    m_keyLayout.setContentsMargins(0,0,0,0);
    m_keyLayout.setSpacing(0);
    m_editLayout.setContentsMargins(0,0,0,0);
    m_editLayout.setSpacing(0);

    m_keyLayout.addWidget(&m_cueName);
    m_keyLayout.addWidget(&m_remove);
    m_layout.addLayout(&m_keyLayout);
    m_layout.addLayout(&m_editLayout);

    m_cueName.setPlaceholderText(QObject::tr("Indice..."));
    connect(&m_cueName, &QLineEdit::editingFinished,
            this, &CueWidget::updateCue);
    for(auto& edit : m_cueEdits)
    {
      m_editLayout.addWidget(&edit);
      connect(&edit, &QLineEdit::editingFinished,
              this, &CueWidget::updateCue);
    }

    m_remove.setText(QObject::tr("Supprimer"));

    connect(&m_remove, &QPushButton::clicked,
            this, &CueWidget::removeRequested);
  }

  void setCue(const SEGMent::Cue& cue)
  {
    m_cue = cue;
    while(m_cue.cues.size() < 3) m_cue.cues.push_back("");

    if(m_cue.key != m_cueName.text())
      m_cueName.setText(cue.key);
    if(m_cue.cues[0] != m_cueEdits[0].text())
      m_cueEdits[0].setText(cue.cues[0]);
    if(m_cue.cues[1] != m_cueEdits[1].text())
      m_cueEdits[1].setText(cue.cues[1]);
    if(m_cue.cues[2] != m_cueEdits[2].text())
      m_cueEdits[2].setText(cue.cues[2]);

  }

  void updateCue()
  {
    if(m_cue.key != m_cueName.text()
       || m_cue.cues[0] != m_cueEdits[0].text()
       || m_cue.cues[1] != m_cueEdits[1].text()
       || m_cue.cues[2] != m_cueEdits[2].text()
       )
    {
      m_cue.key = m_cueName.text();
      m_cue.cues[0] = m_cueEdits[0].text();
      m_cue.cues[1] = m_cueEdits[1].text();
      m_cue.cues[2] = m_cueEdits[2].text();
      cueChanged();
    }
  }

  void cueChanged()
  W_SIGNAL(cueChanged);
  void removeRequested()
  W_SIGNAL(removeRequested);
private:
};

class CueInspectorWidget : public QWidget
{
  const SEGMent::SceneModel& m_scene;
  const score::DocumentContext& m_context;
  Cues m_cues;
  struct Container : public QWidget
  {
      CueInspectorWidget& m_parent;
        Container(CueInspectorWidget& parent)
            : m_parent{parent}
        {
            setMinimumSize(150, 100);
        }

        QSize sizeHint() const override
        {
            return QSize(200, 200 + m_parent.m_widgets.size() * 100);
        }

  };// m_container;

  QVBoxLayout m_layout;
  std::vector<CueWidget*> m_widgets;
  QPushButton m_add;

public:
  CueInspectorWidget(
      const SceneModel& scene
      , const score::DocumentContext& ctx
      , QWidget* parent = nullptr)
    : QWidget{parent}
    , m_scene{scene}
    , m_context{ctx}
    //, m_container{*this}
    , m_layout{this}
  {
    //setWidget(&m_container);
    m_layout.addWidget(&m_add);
    m_layout.addStretch(3);
    m_layout.setContentsMargins(0,0,0,0);
    m_layout.setSpacing(0);

    m_add.setText("+");
    connect(&m_add, &QPushButton::clicked,
            this, [this] {
      newCue();
    });

    setData(scene.cue());
    connect(&scene, &SceneModel::cueChanged,
            this, [this] (const auto& cues) {
        setData(cues);
    });
  }

  void remove(CueWidget* widget)
  {
    ossia::remove_erase(m_widgets, widget);
    m_layout.removeWidget(widget);
    delete widget;

    cuesChanged();
  }

  void cuesChanged()
  {
    Cues newcues;
    for(auto widg : m_widgets)
      newcues.push_back(widg->m_cue);
    if(newcues != m_cues)
    {
      CommandDispatcher<> disp{m_context.commandStack};
      disp.submitCommand(new SetSceneCue{m_scene, std::move(newcues)});
    }
  }

  void newCue()
  {
    auto cue = new CueWidget{};
    connect(cue, &CueWidget::removeRequested,
            this, [this, cue] {
      remove(cue);
    });
    connect(cue, &CueWidget::cueChanged,
            this, &CueInspectorWidget::cuesChanged);

    m_widgets.push_back(cue);
    m_layout.insertWidget(m_layout.count() - 1, cue);
  }

  void setData(const SEGMent::Cues &cues)
  {
    if(m_cues == cues)
        return;
    m_cues = cues;

    if(cues.size() == m_widgets.size())
    {
      for(int i = 0; i < cues.size(); i++)
        m_widgets[i]->setCue(cues[i]);
    }
    else
    {
      if(cues.size() < m_widgets.size())
      {
        // We need to remove some
        while(m_widgets.size() > cues.size())
        {
          delete m_widgets.back();
          m_widgets.pop_back();
        }
      }
      else
      {
        // We need to add some
        while(m_widgets.size() < cues.size())
          newCue();
      }

      for(int i = 0; i < cues.size(); i++)
        m_widgets[i]->setCue(cues[i]);
    }
  }

};
}
