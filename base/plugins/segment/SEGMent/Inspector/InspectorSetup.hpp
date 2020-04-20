#pragma once
#include <Inspector/InspectorLayout.hpp>
#include <Inspector/InspectorWidgetFactoryInterface.hpp>

#include <score/command/Dispatchers/CommandDispatcher.hpp>
#include <score/document/DocumentContext.hpp>
#include <score/widgets/SignalUtils.hpp>

#include <QCheckBox>
#include <QComboBox>
#include <QDropEvent>
#include <QLabel>
#include <QLineEdit>
#include <QList>
#include <QMimeData>
#include <QMovie>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QSlider>
#include <QStackedWidget>
#include <QTableWidget>
#include <QWidget>
//#include <QtColorWidgets/color_wheel.hpp>

#include <SEGMent/Commands/Properties.hpp>
#include <SEGMent/Commands/SoundCommands.hpp>
#include <SEGMent/FilePath.hpp>
#include <SEGMent/Items/GlobalVariables.hpp>
#include <SEGMent/Model/Scene.hpp>
#include <SEGMent/Model/SimpleObject.hpp>

#define INSPECTOR_FACTORY(Factory, InspModel, InspObject, Uuid)               \
  class Factory final : public Inspector::InspectorWidgetFactory              \
  {                                                                           \
    SCORE_CONCRETE(Uuid)                                                      \
  public:                                                                     \
    Factory() = default;                                                      \
                                                                              \
    QWidget* make(                                                            \
        const QList<const QObject*>& sourceElements,                          \
        const score::DocumentContext& doc,                                    \
        QWidget* parent) const override                                       \
    {                                                                         \
      return new InspObject{                                                  \
          safe_cast<const InspModel&>(*sourceElements.first()), doc, parent}; \
    }                                                                         \
                                                                              \
    bool matches(const QList<const QObject*>& objects) const override         \
    {                                                                         \
      return dynamic_cast<const InspModel*>(objects.first());                 \
    }                                                                         \
  };

namespace SEGMent
{
struct EditingFinishedTextEdit final : QPlainTextEdit
{
  W_OBJECT(EditingFinishedTextEdit)
public:
  EditingFinishedTextEdit(QWidget* parent = nullptr) : QPlainTextEdit{parent}
  {
    setTabChangesFocus(true);
    connect(this, &EditingFinishedTextEdit::textChanged, this, [=] {
      m_changed = true;
    });
  }

  void focusOutEvent(QFocusEvent* ev)
  {
    if (m_changed)
      editingFinished();
    QPlainTextEdit::focusOutEvent(ev);
  }

  void setPlainText(const QString& str)
  {
    if (str != toPlainText())
    {
      QPlainTextEdit::setPlainText(str);
      m_changed = false;
    }
  }

  void editingFinished() W_SIGNAL(editingFinished) private : bool m_changed{};
};

/**
 * This class is used to create widgets for the various properties & objects
 * of a SEGMent game.
 */
template <typename T, typename U>
struct WidgetFactory
{
  const U& object;
  const score::DocumentContext& ctx;
  Inspector::Layout& layout;
  QWidget* parent;

  QString prettyText(QString str)
  {
    SCORE_ASSERT(!str.isEmpty());
    str[0] = str[0].toUpper();
    for (int i = 1; i < str.size(); i++)
    {
      if (str[i].isUpper())
      {
        str.insert(i, ' ');
        i++;
      }
    }
    return QObject::tr(str.toUtf8().constData());
  }

  void setup()
  {
    if (auto widg = make((object.*T::get)()); widg != nullptr)
      layout.addRow(prettyText(T::name), (QWidget*)widg);
  }

  void setup(QString txt)
  {
    if (auto widg = make((object.*T::get)()); widg != nullptr)
      layout.addRow(txt, (QWidget*)widg);
  }

  //// Generic widgets ////

  template <typename X>
  auto make(X*) = delete;
  auto make(bool c)
  {
    using cmd =
        typename score::PropertyCommand_T<T>::template command<void>::type;
    auto cb = new QCheckBox{parent};
    cb->setCheckState(c ? Qt::Checked : Qt::Unchecked);

    QObject::connect(
        cb,
        &QCheckBox::stateChanged,
        parent,
        [& object = this->object, &ctx = this->ctx](int state) {
          bool cur = (object.*T::get)();
          if ((state == Qt::Checked && !cur)
              || (state == Qt::Unchecked && cur))
          {
            CommandDispatcher<> disp{ctx.commandStack};
            disp.submitCommand(
                new cmd{object, state == Qt::Checked ? true : false});
          }
        });

    QObject::connect(&object, T::notify, parent, [cb](bool cs) {
      if (cs != cb->checkState())
        cb->setCheckState(cs ? Qt::Checked : Qt::Unchecked);
    });
    return cb;
  }

  auto make(const QString& cur)
  {
    using cmd =
        typename score::PropertyCommand_T<T>::template command<void>::type;
    auto l = new QLineEdit{cur, parent};
    l->setSizePolicy(QSizePolicy::Policy{}, QSizePolicy::MinimumExpanding);
    QObject::connect(
        l,
        &QLineEdit::editingFinished,
        parent,
        [l, &object = this->object, &ctx = this->ctx]() {
          const auto& cur = (object.*T::get)();
          if (l->text() != cur)
          {
            CommandDispatcher<> disp{ctx.commandStack};
            disp.submitCommand(new cmd{object, l->text()});
          }
        });

    QObject::connect(&object, T::notify, parent, [l](const QString& txt) {
      if (txt != l->text())
        l->setText(txt);
    });
    return l;
  }

  auto make(const LongText& cur)
  {
    using cmd =
        typename score::PropertyCommand_T<T>::template command<void>::type;
    auto l = new QPlainTextEdit{cur, parent};
    l->setSizePolicy(QSizePolicy::Policy{}, QSizePolicy::MinimumExpanding);
    QObject::connect(
        l,
        &QPlainTextEdit::textChanged,
        parent,
        [l, &object = this->object, &ctx = this->ctx]() {
          const auto& cur = (object.*T::get)();
          if (auto txt = l->toPlainText(); txt != cur)
          {
            CommandDispatcher<> disp{ctx.commandStack};
            disp.submitCommand(new cmd{object, txt});
          }
        });

    QObject::connect(&object, T::notify, parent, [l](const QString& txt) {
      if (txt != l->toPlainText())
        l->setPlainText(txt);
    });
    return l;
  }


  auto make(const QStringList& cur)
  {
    using cmd =
        typename score::PropertyCommand_T<T>::template command<void>::type;
    auto container = new QWidget;
    auto lay = new QFormLayout{container};
    std::vector<QLineEdit*> edits;
    for(int i = 0; i < cur.size(); i++) {
      auto edit = new QLineEdit;
      edit->setText(cur[i]);
      edits.push_back(edit);
      lay->addRow(QString::number(i) + ":", edit);
    }
    for(QLineEdit* edit : edits) {
      QObject::connect(
          edit,
          &QLineEdit::textChanged,
          parent,
          [edits, &object = this->object, &ctx = this->ctx] () {
            const auto& cur = (object.*T::get)();
            QStringList str;

            for(auto edit : edits)
                str.push_back(edit->text());

            if (str != cur)
            {
              CommandDispatcher<> disp{ctx.commandStack};
              disp.submitCommand(new cmd{object, str});
            }
          });
    }
    QObject::connect(&object, T::notify,
                     parent, [edits] (const QStringList& txt) {
      for(int i = 0; i < txt.size(); i++)
          if (txt[i] != edits[i]->text())
              edits[i]->setText(txt[i]);
    });
    return container;
  }
/*
  auto make(QColor cur)
  {
    using cmd =
        typename score::PropertyCommand_T<T>::template command<void>::type;
    auto l = new color_widgets::ColorWheel{parent};
    l->setColor(cur);
    QObject::connect(
        l,
        &color_widgets::ColorWheel::colorChanged,
        parent,
        [l, &object = this->object, &ctx = this->ctx](QColor c) {
          const auto& cur = (object.*T::get)();
          if (l->color() != cur)
          {
            CommandDispatcher<> disp{ctx.commandStack};
            disp.submitCommand(new cmd{object, l->color()});
          }
        });

    QObject::connect(&object, T::notify, parent, [l](QColor c) {
      if (c != l->color())
        l->setColor(c);
    });
    return l;
  }
*/
  //// Specialized widgets ////
  struct SoundLineEdit : QLineEdit
  {
    SoundLineEdit(QString txt = {}, QWidget* parent = nullptr)
        : QLineEdit{txt, parent}
    {
      setMinimumWidth(175);
      setClearButtonEnabled(true);
    }

    void dropEvent(QDropEvent* e) override
    {
      if (auto mime = e->mimeData(); mime->text() == SEGMENT_SOUND_ID)
      {
        QUrl currentUrl = mime->urls().first();
        setText(currentUrl.path());
      }
      e->accept();
    }
  };

  auto make(const SEGMent::Sound& cur)
  {
    using cmd =
        typename score::PropertyCommand_T<T>::template command<void>::type;
    const Sound& sound_obj
        = (const_cast<U&>(object).*SoundAccessor<U>::value)();

    auto w = new QWidget;
    w->setContentsMargins(0, 0, 0, 0);
    auto lay = new score::MarginLess<QVBoxLayout>{w};

    // Sound path
    {
      auto l = new SoundLineEdit{cur.path(), w};
      l->setSizePolicy(QSizePolicy::Policy{}, QSizePolicy::MinimumExpanding);
      lay->addWidget(l);

      QObject::connect(
          l,
          &QLineEdit::textChanged,
          [l, &object = this->object, &ctx = this->ctx](const auto& txt) {
            if (!l->hasFocus())
            {
              CommandDispatcher<> disp{ctx.commandStack};
              Sound s = (object.*T::get)();
              if (s.path() != txt)
              {
                s.setPath(txt);
                disp.submitCommand(new cmd{object, std::move(s)});
              }
            }
          });
      QObject::connect(
          l,
          &QLineEdit::editingFinished,
          parent,
          [l, &object = this->object, &ctx = this->ctx]() {
            CommandDispatcher<> disp{ctx.commandStack};
            Sound s = (object.*T::get)();
            if (s.path() != l->text())
            {
              s.setPath(l->text());
              disp.submitCommand(new cmd{object, std::move(s)});
            }
          });

      QObject::connect(
          &cur, &Sound::pathChanged, parent, [l](const QString& txt) {
            if (txt != l->text())
              l->setText(txt);
          });
    }

    // Play - stop - Repeat
    {
      auto sub = new score::MarginLess<QHBoxLayout>;
      lay->addLayout(sub);

      {
        auto play = new QPushButton{};
        play->setIcon(
            QIcon{qApp->style()->standardPixmap(QStyle::SP_MediaPlay)});
        QObject::connect(
            play,
            &QPushButton::pressed,
            parent,
            [& object = this->object, &ctx = this->ctx] {
              SoundPlayer::instance().play((object.*T::get)(), ctx);
            });
        play->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
        sub->addWidget(play);
      }

      {
        auto stop = new QPushButton{};
        stop->setIcon(
            QIcon{qApp->style()->standardPixmap(QStyle::SP_MediaStop)});
        QObject::connect(stop, &QPushButton::pressed, parent, [] {
          SoundPlayer::instance().stop();
        });
        stop->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
        sub->addWidget(stop);
      }
      sub->addStretch(1);

      auto rpt = new QCheckBox{QObject::tr("Repeat")};
      rpt->setChecked(sound_obj.repeat());
      QObject::connect(
          rpt,
          &QCheckBox::stateChanged,
          parent,
          [& ctx = this->ctx, &object = this->object](int st) {
            CommandDispatcher<> disp{ctx.commandStack};
            Sound s = (object.*T::get)();
            if (s.repeat() != bool(st))
            {
              s.setRepeat(bool(st));
              disp.submitCommand(new cmd{object, std::move(s)});
            }
          });
      rpt->setSizePolicy(
          QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
      lay->addWidget(rpt);
    }

    // Volume
    {
      auto sl = new QSlider;
      sl->setRange(0, 100);
      sl->setValue(sound_obj.volume() * 100.);
      sl->setOrientation(Qt::Horizontal);
      QObject::connect(
          sl,
          &QSlider::sliderMoved,
          parent,
          [& object = this->object, &ctx = this->ctx](int volume) {
            SoundPlayer::instance().setVolume(volume);
            ctx.dispatcher.template submitCommand<SetVolume<U>>(
                object, volume / 100.);
          });

      QObject::connect(
          sl, &QSlider::sliderReleased, parent, [& ctx = this->ctx] {
            ctx.dispatcher.commit();
          });
      sl->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
      lay->addWidget(sl);
    }

    return w;
  }

  auto make(const SEGMent::GifModel::Frames& cur)
  {
    auto widg = new QTableWidget;
    QObject::connect(
        widg,
        &QTableWidget::itemDoubleClicked,
        &object,
        [& object = this->object, &ctx = this->ctx](QTableWidgetItem* item) {
          CommandDispatcher<> disp{ctx.commandStack};
          if (item->column() == 0)
            disp.submitCommand(new SetGifDefaultFrame{object, item->row()});
        });

    widg->setRowCount(cur.size());
    widg->setColumnCount(2);
    QMovie v(toLocalFile(this->object.image(), ctx));
    v.setCacheMode(QMovie::CacheAll);
    for (std::size_t i = 0; i < cur.size(); i++)
    {
      v.jumpToNextFrame();
      auto image = new QTableWidgetItem;
      image->setData(Qt::DecorationRole, v.currentPixmap().scaledToWidth(120));
      image->setFlags(image->flags() ^ Qt::ItemIsEditable);
      widg->setItem(i, 0, image);

      auto cb = new QComboBox{};
      cb->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
      cb->addItems({QObject::tr("Passée"),
                    QObject::tr("Stop"),
                    QObject::tr("Victorieuse")});
      cb->setCurrentIndex((int)cur[i]);
      QObject::connect(
          cb,
          SignalUtils::QComboBox_currentIndexChanged_int(),
          &object,
          [i, &object = this->object, &ctx = this->ctx](int idx) {
            using cmd = typename score::PropertyCommand_T<T>::template command<
                void>::type;

            CommandDispatcher<> disp{ctx.commandStack};
            auto f = object.frames();
            f[i] = (SEGMent::GifModel::Frame)idx;
            disp.submitCommand(new cmd{object, std::move(f)});
          });

      widg->setCellWidget(i, 1, cb);
    }

    widg->resizeColumnToContents(0);
    widg->resizeRowsToContents();

    return widg;
  }

  //// Transitions ////

  static inline const SEGMent::riddle_t& get_riddle(const U& object) {
    return object.transition()
                 .template target<SEGMent::SceneToScene>()
                 ->riddle;
  }

  auto operator()(const SEGMent::SceneToScene& cur)
  {
    {
      layout.addRow(new QWidget);
      QFrame* line = new QFrame();
      line->setFrameShape(QFrame::HLine);
      line->setFrameShadow(QFrame::Sunken);

      layout.addRow(line);
    }

    // Timing
    {
      auto time = new QSpinBox;
      time->setRange(0, 1000);
      time->setSpecialValueText("Aucun");
      time->setValue(60);
      time->setSuffix(QObject::tr(" seconds"));
      if(cur.riddle.maxTime)
      {
        time->setValue(*cur.riddle.maxTime);
      }
      else
      {
        time->setValue(0);
      }

      QObject::connect(time, qOverload<int>(&QSpinBox::valueChanged),
              parent, [&object = this->object, &ctx = this->ctx] (int time) {
        SEGMent::riddle_t cur = get_riddle(object);
        if (cur.maxTime != time)
        {
          if(time == 0)
            cur.maxTime = ossia::none;
          else
          {
            cur.maxTime = time;
          }


          CommandDispatcher<> disp{ctx.commandStack};
          disp.submitCommand(new ChangeRiddle{object, cur});
        }
      });

      layout.addRow(QObject::tr("Temps limite"), time);
    }

    // Riddle choosing
    auto cb = new QComboBox;
    cb->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    cb->addItem(QObject::tr("None"));
    cb->addItem(QObject::tr("Gif"));
    cb->addItem(QObject::tr("Puzzle"));
    cb->addItem(QObject::tr("Text"));

    if (!cur.riddle)
      cb->setCurrentIndex(0);
    else
      cb->setCurrentIndex(1 + cur.riddle.which());

    QObject::connect(
        cb,
        SignalUtils::QComboBox_currentIndexChanged_int(),
        parent,
        [& object = this->object, &ctx = this->ctx](int idx) {
          CommandDispatcher<> disp{ctx.commandStack};
          auto riddle = get_riddle(object);
          switch (idx)
          {
            case 0:
            {
              if (riddle.which() != riddle.npos)
              {
                static_cast<riddle_t::impl_type&>(riddle) = riddle_t::impl_type{};
                disp.submitCommand(new ChangeRiddle{object, std::move(riddle)});
              }
              break;
            }
            case 1:
            {
              if (riddle.which() != 0)
              {
                static_cast<riddle_t::impl_type&>(riddle) = SEGMent::GifRiddle{};
                disp.submitCommand(new ChangeRiddle{object, std::move(riddle)});
              }
              break;
            }
            case 2:
            {
              if (riddle.which() != 1)
              {
                static_cast<riddle_t::impl_type&>(riddle) = SEGMent::PuzzleRiddle{};
                disp.submitCommand(new ChangeRiddle{object, std::move(riddle)});
              }
              break;
            }
            case 3:
            {
              if (riddle.which() != 2)
              {
                static_cast<riddle_t::impl_type&>(riddle) = SEGMent::TextRiddle{};
                disp.submitCommand(new ChangeRiddle{object, std::move(riddle)});
              }
              break;
            }
          }
        });

    layout.addRow(QObject::tr("Riddle"), cb);

    {
    auto sw = new QStackedWidget;
    // nothing
    sw->addWidget(new QWidget);

    // gif
    sw->addWidget(new QWidget);

    // puzzle
    sw->addWidget(new QWidget);

    // text
    auto tw = new QWidget;
    auto tl = new Inspector::Layout{tw};

    auto question = new QLineEdit;
    {
      QObject::connect(
          question,
          &QLineEdit::editingFinished,
          parent,
          [question, &object = this->object, &ctx = this->ctx] {
            auto cur = get_riddle(object);
            auto& riddle = *cur.template target<TextRiddle>();
            if (riddle.question != question->text())
            {
              riddle.question = question->text();

              CommandDispatcher<> disp{ctx.commandStack};
              disp.submitCommand(new ChangeRiddle{object, cur});
            }
          });
    }
    auto expected = new EditingFinishedTextEdit;
    {
      QObject::connect(
          expected,
          &EditingFinishedTextEdit::editingFinished,
          parent,
          [expected, &object = this->object, &ctx = this->ctx] {
            auto cur = get_riddle(object);
            auto& riddle = *cur.template target<TextRiddle>();
            auto old_riddle = riddle;
            auto str = expected->toPlainText();
            auto answers
                = str.split("\n", QString::SplitBehavior::SkipEmptyParts);

            switch (answers.size())
            {
              case 0:
                riddle.expected.clear();
                riddle.fuzzyMatches.clear();
                break;
              case 1:
                riddle.expected = answers[0];
                riddle.fuzzyMatches.clear();
                break;
              default:
                riddle.expected = answers[0];
                answers.removeFirst();
                riddle.fuzzyMatches = answers;
                break;
            }

            if (old_riddle != riddle)
            {
              CommandDispatcher<> disp{ctx.commandStack};
              disp.submitCommand(new ChangeRiddle{object, cur});
            }
          });
    }
    auto ifCorrect = new QLineEdit;
    {
      QObject::connect(
          ifCorrect,
          &QLineEdit::editingFinished,
          parent,
          [ifCorrect, &object = this->object, &ctx = this->ctx] {
            auto cur = get_riddle(object);
            auto& riddle = *cur.template target<TextRiddle>();
            if (riddle.ifCorrect != ifCorrect->text())
            {
              riddle.ifCorrect = ifCorrect->text();

              CommandDispatcher<> disp{ctx.commandStack};
              disp.submitCommand(new ChangeRiddle{object, cur});
            }
          });
    }
    auto ifWrong = new QLineEdit;
    {
      QObject::connect(
          ifWrong,
          &QLineEdit::editingFinished,
          parent,
          [ifWrong, &object = this->object, &ctx = this->ctx] {
            auto cur = get_riddle(object);
            auto& riddle = *cur.template target<TextRiddle>();
            if (riddle.ifWrong != ifWrong->text())
            {
              riddle.ifWrong = ifWrong->text();

              CommandDispatcher<> disp{ctx.commandStack};
              disp.submitCommand(new ChangeRiddle{object, cur});
            }
          });
    }

    auto stars = new QCheckBox(QObject::tr("Password (******)"));
    {
      QObject::connect(
          stars,
          &QCheckBox::stateChanged,
          parent,
          [&object = this->object, &ctx = this->ctx] (int state) {
            auto cur = get_riddle(object);
            auto& riddle = *cur.template target<TextRiddle>();

            riddle.useStars = bool(state);

            CommandDispatcher<> disp{ctx.commandStack};
            disp.submitCommand(new ChangeRiddle{object, cur});
          });
    }

    tl->addRow(QObject::tr("Question"), question);
    tl->addRow(QObject::tr("Expected\nanswers\n(1 per line)"), expected);
    tl->addRow(QObject::tr("OK text"), ifCorrect);
    tl->addRow(QObject::tr("Wrong text"), ifWrong);

    tl->addRow(stars);

    // TODO fuzzy matches
    if (auto tr = cur.riddle.target<TextRiddle>())
    {
      question->setText(tr->question);
      expected->setPlainText(tr->expected);
      for (auto str : tr->fuzzyMatches)
      {
        expected->appendPlainText(str);
      }
      ifCorrect->setText(tr->ifCorrect);
      ifWrong->setText(tr->ifWrong);
    }

    sw->addWidget(tw);
    layout.addRow(sw);

    auto set_sw_index = [=, &cur](int index) {
      switch (index)
      {
        case 0:
          cb->setCurrentIndex(1);
          sw->setCurrentIndex(1);
          break;
        case 1:
          cb->setCurrentIndex(2);
          sw->setCurrentIndex(2);
          break;
        case 2:
          cb->setCurrentIndex(3);
          sw->setCurrentIndex(3);
          if (auto tr = cur.riddle.target<TextRiddle>())
          {
            question->setText(tr->question);
            expected->setPlainText(tr->expected);
            for (auto str : tr->fuzzyMatches)
            {
              expected->appendPlainText(str);
            }
            ifCorrect->setText(tr->ifCorrect);
            ifWrong->setText(tr->ifWrong);
          }
          break;
        default:
          cb->setCurrentIndex(0);
          sw->setCurrentIndex(0);
          break;
      }
    };

    set_sw_index(cur.riddle.which());

    QObject::connect(
        &object, T::notify, parent, [=](const transition_t& new_t) {
          set_sw_index(new_t.target<SceneToScene>()->riddle.which());
        });
    }

    return nullptr;
  }

  auto operator()(const SEGMent::ObjectToScene& cur) { return nullptr; }
  auto operator()(const SEGMent::GifToScene& cur) { return nullptr; }
  auto operator()(const SEGMent::ClickToScene& cur) { return nullptr; }

  auto make(const SEGMent::transition_t& cur)
  {
    return eggs::variants::apply(*this, cur);
  }

  //// Enums ////

  template <typename Type>
  auto make_combo(std::vector<std::pair<QString, Type>> values, Type s)
  {
    using cmd =
        typename score::PropertyCommand_T<T>::template command<void>::type;
    auto cb = new QComboBox;
    cb->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    for (auto& v : values)
    {
      cb->addItem(v.first, v.second);
    }

    cb->setCurrentIndex(static_cast<int>(s));
    QObject::connect(
        cb,
        SignalUtils::QComboBox_currentIndexChanged_int(),
        parent,
        [cb, &object = this->object, &ctx = this->ctx](int idx) {
          auto data = cb->itemData(idx).value<Type>();
          if (data != (object.*T::get)())
          {
            CommandDispatcher<> disp{ctx.commandStack};
            disp.submitCommand(new cmd{object, data});
          }
        });

    QObject::connect(&object, T::notify, parent, [cb](auto val) {
      auto idx = static_cast<int>(val);
      if (idx >= 0 && idx < cb->count() && idx != cb->currentIndex())
      {
        cb->blockSignals(true);
        cb->setCurrentIndex(idx);
        cb->blockSignals(false);
      }
    });
    return cb;
  }

  auto make(TransitionModel::FadeMode f)
  {
    return make_combo(
        {{QObject::tr("Immediate"), TransitionModel::FadeMode::Immediate},
         {QObject::tr("Crossfade"), TransitionModel::FadeMode::Crossfade}},
        f);
  }

  auto make(SceneModel::SceneType s)
  {
    return make_combo(
        {{QObject::tr("Default"), SceneModel::SceneType::Default},
         {QObject::tr("Initial"), SceneModel::SceneType::Initial},
         {QObject::tr("Final"), SceneModel::SceneType::Final},
         {QObject::tr("Game Over"), SceneModel::SceneType::GameOver}},
        s);
  }

  auto make(TextAreaModel::Behaviour s)
  {
    return make_combo(
        {{QObject::tr("Add"), TextAreaModel::Behaviour::Add},
         {QObject::tr("Replace"), TextAreaModel::Behaviour::Replace},
         {QObject::tr("Clear"), TextAreaModel::Behaviour::Clear},
         {QObject::tr("Validate"), TextAreaModel::Behaviour::Validate}},
        s);
  }
};

template <typename T, typename U>
auto setup_inspector(
    T,
    const U& sc,
    const score::DocumentContext& doc,
    Inspector::Layout& layout,
    QWidget* parent)
{
  WidgetFactory<T, U>{sc, doc, layout, parent}.setup();
}

template <typename T, typename U>
auto setup_inspector(
    T,
    QString txt,
    const U& sc,
    const score::DocumentContext& doc,
    Inspector::Layout& layout,
    QWidget* parent)
{
  WidgetFactory<T, U>{sc, doc, layout, parent}.setup(txt);
}
} // namespace SEGMent
