#include "AboutDialog.hpp"

#include <score/widgets/Pixmap.hpp>

#include <QDate>
#include <QDebug>
#include <QDesktopServices>
#include <QEvent>
#include <QFont>
#include <QListWidget>
#include <QPainter>
#include <QPicture>
#include <QPlainTextEdit>
#include <QSize>

#include <score_git_info.hpp>
#include <score_licenses.hpp>

#include <map>
namespace score
{
AboutDialog::AboutDialog(QWidget* parent)
    : QDialog(parent)
    , m_windowSize(500, 537)
{
  setWindowFlag(Qt::FramelessWindowHint);
  resize(m_windowSize.width(), m_windowSize.height());
  setMouseTracking(true);

  // map
  struct License
  {
    License() = default;
    License(const License&) = default;
    License(License&&) = default;
    License& operator=(const License&) = default;
    License& operator=(License&&) = default;
    License(QString u, QString l) : url{u}, license{l} {}
    License(QString u, const unsigned char* l) : url{u}
    {
      license = QString::fromUtf8(reinterpret_cast<const char*>(l));
    }
    License(QString u, QString lstart, const unsigned char* l)
        : url{u}, license{lstart}
    {
      license += QString::fromUtf8(reinterpret_cast<const char*>(l));
    }
    QString url;
    QString license;
  };

  std::map<QString, License> map;
  map["Score"] = License{"https://ossia.io", score_LICENSE};

  map["libossia"] = License{"https://ossia.io", ossia_LICENSE};

  map["Qt"] = License{"https://www.qt.io", "GNU General Public License v3"};

  map["Boost"]
      = License{"https://www.boost.org", "Boost Software License 1.0"};

  // In libossia
  map["Asio"] = License{"https://github.com/chriskohlhoff/asio", Asio_LICENSE};
  map["CMake"] = License{"https://www.cmake.org", "BSD 3-clause License"};
  map["TinySpline"]
      = License{"https://github.com/msteinbeck/tinyspline", "MIT License"};
  map["GSL"] = License{"https://github.com/Microsoft/GSL", GSL_LICENSE};

  map["RtMidi17"]
      = License{"https://github.com/jcelerier/RtMidi17",
                "Based on RtMidi (https://github.com/thestk/rtmidi) and "
                "ModernMIDI (https://github.com/ddiakopoulos/ModernMIDI)"};
  map["RtMidi"] = License{"https://github.com/thestk/rtmidi", rtmidi_LICENSE};
  map["ModernMidi"] = License{"(https://github.com/ddiakopoulos/ModernMIDI",
                              modernmidi_LICENSE};
  map["Servus"] = License{"https://github.com/jcelerier/Servus",
                          "Based on https://github.com/HBPVIS/Servus\n",
                          servus_LICENSE};

  map["SmallFunction"]
      = License{"https://github.com/jcelerier/SmallFunction",
                "Based on https://github.com/LoopPerfect/smallfunction\n",
                smallfun_LICENSE};

  map["bitset2"]
      = License{"https://github.com/ClaasBontus/bitset2", bitset2_LICENSE};
  map["Brigand"]
      = License{"https://github.com/edouarda/brigand", Brigand_LICENSE};
  map["Chobo"]
      = License{"https://github.com/Chobolabs/chobo-shl", chobo_LICENSE};
  map["ConcurrentQueue"]
      = License{"https://github.com/cameron314/concurrentqueue",
                concurrentqueue_LICENSE};
  map["ReaderWriterQueue"]
      = License{"https://github.com/cameron314/readerwriterqueue",
                readerwriterqueue_LICENSE};
  map["flat"] = License{"https://github.com/jcelerier/flat",
                        "Based on https://github.com/pubby/flat\n",
                        flat_LICENSE};
  map["flat_hash_map"]
      = License{"https://github.com/jcelerier/flat_hash_map",
                "Based on https://github.com/skarupke/flat_hash_map\n"};
  map["fmt"] = License{"https://github.com/fmtlib/fmt", fmt_LICENSE};
  map["frozen"]
      = License{"https://github.com/serge-sans-paille/frozen", frozen_LICENSE};
  map["hopscotch-map"] = License{"https://github.com/tessil/hopscotch-map",
                                 hopscotchmap_LICENSE};
  map["multi_index"] = License{"https://github.com/jcelerier/multi_index",
                               multiindex_LICENSE};
  map["nano-signal-slot"] = License{
      "https://github.com/jcelerier/nano-signal-slot", nanosignal_LICENSE};
  map["OSCPack"]
      = License{"https://github.com/jcelerier/oscpack", "Boost License"};
  map["Pure Data"]
      = License{"https://github.com/pure-data/pure-data.git", pd_LICENSE};
  map["pybind11"]
      = License{"https://github.com/pybind/pybind11", pybind11_LICENSE};
  map["rapidjson"]
      = License{"https://github.com/miloyip/rapidjson", rapidjson_LICENSE};
  map["spdlog"] = License{"https://github.com/gabime/spdlog", spdlog_LICENSE};
  map["variant"]
      = License{"https://github.com/eggs-cpp/variant", variant_LICENSE};
  map["verdigris"]
      = License{"https://github.com/jcelerier/verdigris", verdigris_LICENSE};
  map["weakjack"] = License{"https://github.com/jcelerier/weakjack",
                            "Based on https://github.com/x42/weakjack\nGNU "
                            "General Public License version 2 (or later)"};
  map["websocketpp"] = License{"https://github.com/jcelerier/websocketpp",
                               websocketpp_LICENSE};
  //map["whereami"]
  //    = License{"https://github.com/gpakosz/whereami", whereami_LICENSE};

  // In score
  map["QMenuView"]
      = License{"https://github.com/pvanek/qlipper/blob/master/qmenuview",
                "CeCILL license. \nXINX\nCopyright Ulrich Van Den Hekke, "
                "(2007-2011)\nxinx@shadoware.org"};
  map["PSIMPL"] = License{"http://psimpl.sf.net", "MPL 1.1"};
  map["miniz"] = License{"https://github.com/richgel999/miniz", "MIT License"};
  map["desktopqqc2style"]
      = License{"https://anongit.kde.org/scratch/mart/desktopqqc2style.git",
                "MIT License"};

  // TODO ifdefs
  map["SDL"] = License{"", "Boost License"};
  map["Faust"]
      = License{"https://faust.grame.fr", "GNU General Public License"};
  map["PortAudio"] = License{"https://www.portaudio.com", portaudio_LICENSE};
  map["libJACK"]
      = License{"https://jackaudio.org", "GNU Lesser General Public License"};
  map["Phidgets"] = License{"https://www.phidgets.com/", "Boost License"};
  map["FFMPEG"]
      = License{"https://ffmpeg.org", "GNU General Public License v3"};
  map["libusb"]
      = License{"https://libusb.info/",
                "GNU Lesser General Public License v2.1 or any later version"};

  // software list
  auto softwareList = new QListWidget{this};
  softwareList->move(5, 260);
  softwareList->resize(120, 253);
  softwareList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  softwareList->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

  for (const auto& item : map)
  {
    softwareList->addItem(item.first);
  }
  for (std::size_t i = 0; i < map.size(); i++)
  {
    softwareList->item(i)->setTextAlignment(Qt::AlignHCenter);
  }

  // license
  auto license = new QPlainTextEdit{this};
  license->move(140, 260);
  license->resize(355, 253);
  license->setReadOnly(true);
  connect(
      softwareList,
      &QListWidget::currentTextChanged,
      this,
      [=](const QString& currentText) {
        auto& lic = map.at(currentText);
        license->setPlainText(lic.url + "\n\n" + lic.license);
      });
}

void AboutDialog::mousePressEvent(QMouseEvent* event)
{
  this->close();
}

void AboutDialog::mouseMoveEvent(QMouseEvent* event)
{
}
void AboutDialog::paintEvent(QPaintEvent* event)
{
  QPen textPen(QColor("#00000"));
  QPen titleText(QColor("#222222"));

  // draw background image
  QPainter painter(this);

  // write version and commit
  {
    auto version_text = QStringLiteral(
          "SEGMent - Version: %1.%2\n").arg(SCORE_VERSION_MAJOR).arg(SCORE_VERSION_MINOR);


#if defined(_WIN32)
    painter.setFont(QFont("Sans", 10, QFont::Bold));
#else
    painter.setFont(QFont("Sans", 12, QFont::Bold));
#endif
    painter.setPen(textPen);
    painter.drawText(
        QRectF(0, 20, m_windowSize.width(), 60),
        Qt::AlignHCenter,
        version_text);

    QString commit{GIT_COMMIT};

    if (!commit.isEmpty())
    {
#if defined(_WIN32)
    painter.setFont(QFont("Sans", 9));
#else
    painter.setFont(QFont("Sans", 11));
#endif
        painter.setPen(textPen);
        painter.drawText(
            QRectF(0, 40, m_windowSize.width(), 60),
            Qt::AlignHCenter,
            tr("Commit: %1").arg(commit));
    }
  }

  // write copyright
  {
    auto credits_text = QString::fromUtf8(
          "\nD'après une idée originale de Raphaël Marczak.\n\n"
          "Développement : Jean-Michaël Celerier, Raphaël Marczak, Vincent Casamayou.\n"
          "Contributions : Gyorgy Kurtag, Frédéric Bouyssi, et de nombreux stagiaires.\n"
          "L'éditeur SEGMent est basé sur le logiciel ossia score (https://ossia.io).\n\n"
          "Développement financé par : \n "
          "Le pôle documentaire de l'Université de Bordeaux\n"
          "Le SCRIME (https://scrime.labri.fr)\n"
          "L'IdEx\n"
          );

#if defined(_WIN32)
    painter.setFont(QFont("Sans", 8));
#else
    painter.setFont(QFont("Sans", 10));
#endif
    painter.drawText(
        QRectF(0, 60, m_windowSize.width(), 200),
        Qt::AlignHCenter,
        credits_text);
  }

  // write title above listview
  painter.setPen(titleText);
  painter.drawText(QRectF(5, 240, 120, 15), Qt::AlignHCenter, "Projet");

  // write title above license
  painter.drawText(QRectF(140, 240, 185, 15), Qt::AlignHCenter, "License");
}

} // namespace score
