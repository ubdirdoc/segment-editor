#include <core/document/Document.hpp>

#include <QApplication>
#include <QBrush>
#include <QColor>
#include <QDir>
#include <QDrag>
#include <QFormLayout>
#include <QLabel>
#include <QListWidget>
#include <QMimeData>
#include <QMouseEvent>
#include <QPushButton>
#include <QSplitter>

#include <SEGMent/Items/GlobalVariables.hpp>
#include <SEGMent/Panel/Library.hpp>
#include <wobjectimpl.h>
W_OBJECT_IMPL(SEGMent::Library)
namespace SEGMent
{
struct TextLabel : public QLabel
{
  using QLabel::QLabel;
  void mousePressEvent(QMouseEvent* event) override { event->ignore(); }

  void mouseMoveEvent(QMouseEvent* event) override { event->ignore(); }

  void mouseReleaseEvent(QMouseEvent* event) override { event->ignore(); }
};

//! A button which can be dragged from.
struct DraggableButton final : public QLabel
{
  QString m_mime;
  DraggableButton(QString txt, QString mime, QWidget* parent)
      : QLabel{parent}, m_mime{mime}
  {
    setText(txt);
    setWordWrap(true);
    setAlignment(Qt::AlignCenter);
    setStyleSheet(
        "QLabel{ "
        "  border-radius: 10px; "
        "  background: #ddd; "
        "  color: #000;"
        "}");
    setFixedSize(200, 100);
    setFont(QFont("Helvetica", 15));
  }

  void enterEvent(QEvent* event) override
  {
    QFont f = font();
    f.setUnderline(true);
    setFont(f);
  }

  void leaveEvent(QEvent* event) override
  {
    QFont f = font();
    f.setUnderline(false);
    setFont(f);
  }

  void mousePressEvent(QMouseEvent* event) override
  {
    auto drag = new QDrag(this);
    auto mimeData = new QMimeData;
    mimeData->setData("segment/drag", m_mime.toLatin1());
    mimeData->setText(m_mime.toLatin1());

    QPoint point = event->pos();
    drag->setMimeData(mimeData);
    drag->setHotSpot(point);

    drag->exec(Qt::MoveAction);
    QLabel::mousePressEvent(event);
  }

  void mouseMoveEvent(QMouseEvent* event) override
  {
    QLabel::mouseMoveEvent(event);
  }

  void mouseReleaseEvent(QMouseEvent* event) override
  {
    QLabel::mouseReleaseEvent(event);
  }
};

Library::Library(const score::GUIApplicationContext& ctx)
    : score::PanelDelegate{ctx}
{
  auto ressourcesPath
      = QFileInfo(qApp->applicationFilePath()).absolutePath() + "/Ressources";
  m_widget = new QTabWidget;

  auto imgs = new QTabWidget;
  auto snds = new QTabWidget;
  m_widget->addTab(imgs, tr("Images"));
  m_widget->addTab(snds, tr("Sounds"));

  {
    m_zones = new QWidget;
    auto lay = new QVBoxLayout{m_zones};
    lay->setAlignment(Qt::AlignCenter);

    lay->addWidget(
        new DraggableButton{tr("Click Area"), SEGMENT_CLICKAREA_ID, m_zones});
    lay->addWidget(new DraggableButton{
        tr("Back Click Area"), SEGMENT_BACKCLICKAREA_ID, m_zones});
    lay->addWidget(
        new DraggableButton{tr("Text Area"), SEGMENT_TEXTAREA_ID, m_zones});
    lay->addWidget(new QSplitter);

    lay->addWidget(new DraggableButton{
        tr("Text riddle"), SEGMENT_TEXTRIDDLE_ID, m_zones});
    lay->addWidget(
        new DraggableButton{tr("Gif riddle"), SEGMENT_GIFRIDDLE_ID, m_zones});
    lay->addWidget(new DraggableButton{
        tr("Puzzle riddle"), SEGMENT_PUZZLERIDDLE_ID, m_zones});
    lay->addStretch();

    m_widget->addTab(m_zones, tr("Zones"));
  }

  {
    m_scenes = new ImageList(
        ressourcesPath, SCENE_IMAGES_DIRECTORY, SEGMENT_SCENE_ID);
    m_scenes->setSpacing(10);
    m_scenes->setViewMode(QListWidget::IconMode);
    m_scenes->setIconSize(QSize(100, 100));
    m_scenes->setResizeMode(QListWidget::Adjust);
    m_scenes->setGridSize({110, 115});

    m_scenes->reloadList();

    imgs->addTab(m_scenes, tr("Scenes"));
  }

  {
    m_objects = new ImageList(
        ressourcesPath, OBJECT_IMAGES_DIRECTORY, SEGMENT_OBJECT_ID);

    m_objects->setSpacing(10);
    m_objects->setViewMode(QListWidget::IconMode);
    m_objects->setIconSize(QSize(100, 100));
    m_objects->setResizeMode(QListWidget::Adjust);
    m_objects->setGridSize({110, 115});

    m_objects->reloadList();

    imgs->addTab(m_objects, tr("Objects"));
  }

  {
    m_templates = new TemplateList(
        ressourcesPath, TEMPLATES_DIRECTORY, SEGMENT_TEMPLATE_ID);
    m_templates->setSpacing(10);
    m_templates->setViewMode(QListWidget::IconMode);
    m_templates->setIconSize(QSize(100, 100));
    m_templates->setResizeMode(QListWidget::Adjust);
    m_templates->setGridSize({110, 115});

    m_templates->reloadList();
    imgs->addTab(m_templates, tr("Templates"));
  }

  {
    m_sounds
        = new SoundList(ressourcesPath, SOUNDS_DIRECTORY, SEGMENT_SOUND_ID);
    m_sounds->reloadList();

    snds->addTab(m_sounds, tr("Sounds"));
  }
}

QWidget* Library::widget()
{
  return m_widget;
}

const score::PanelStatus& Library::defaultPanelStatus() const
{
  static const score::PanelStatus status{true,
                                         Qt::LeftDockWidgetArea,
                                         10,
                                         QObject::tr("SEGMent Library"),
                                         QObject::tr("Ctrl+U")};

  return status;
}

void Library::on_modelChanged(
    score::MaybeDocument oldm,
    score::MaybeDocument newm)
{
  if (oldm)
  {
    disconnect(
        &oldm->document.metadata(),
        &score::DocumentMetadata::fileNameChanged,
        this,
        &Library::on_documentPathChanged);
  }
  if (newm)
  {
    connect(
        &newm->document.metadata(),
        &score::DocumentMetadata::fileNameChanged,
        this,
        &Library::on_documentPathChanged);
    on_documentPathChanged(newm->document.metadata().fileName());
  }
  else
  {
    auto ressourcesPath = QFileInfo(qApp->applicationFilePath()).absolutePath()
                          + "/Ressources";

    m_scenes->reloadList(ressourcesPath);
    m_objects->reloadList(ressourcesPath);
    m_sounds->reloadList(ressourcesPath);
    m_templates->reloadList(ressourcesPath);
  }
}

void Library::on_documentPathChanged(const QString& path)
{
  QFileInfo file(path);
  if (file.exists())
  {
    auto folder = file.absolutePath();
    auto res_path = folder + "/Ressources";
    if (QDir{res_path}.exists())
    {
      m_scenes->reloadList(res_path);
      m_objects->reloadList(res_path);
      m_sounds->reloadList(res_path);
      m_templates->reloadList(res_path);
    }
    else
    {
      m_scenes->reloadList(folder);
      m_objects->reloadList(folder);
      m_sounds->reloadList(folder);
      m_templates->reloadList(folder);
    }
  }
  else
  {
    m_scenes->clear();
    m_objects->clear();
    m_sounds->clear();
    m_templates->clear();
  }
}

void Library::setNewSelection(const Selection& s) {}

std::unique_ptr<score::PanelDelegate>
LibraryFactory::make(const score::GUIApplicationContext& ctx)
{
  return std::make_unique<Library>(ctx);
}
} // namespace SEGMent
