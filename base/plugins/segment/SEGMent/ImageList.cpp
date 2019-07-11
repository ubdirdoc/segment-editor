#include <QApplication>
#include <QBuffer>
#include <QDebug>
#include <QDir>
#include <QDrag>
#include <QFileSystemModel>
#include <QFileSystemWatcher>
#include <QJsonDocument>
#include <QMimeData>
#include <QMouseEvent>
#include <QPainter>
#include <QShortcut>

#include <SEGMent/ImageList.hpp>
#include <SEGMent/Items/GlobalVariables.hpp>
#include <SEGMent/Model/Sound.hpp>
#include <SEGMent/ImageCache.hpp>
namespace SEGMent
{
ImageList::ImageList(QString resPath, QString folder, QString id)
    : m_path{resPath}, m_folder{folder}, m_id{id}
{
  connect(
      &m_watch,
      &QFileSystemWatcher::directoryChanged,
      this,
      &ImageList::loadFolder);
}

void ImageList::startDrag(Qt::DropActions supportedActions)
{
  QListWidget::startDrag(supportedActions);
}

void ImageList::dragEnterEvent(QDragEnterEvent* event) {}

void ImageList::reloadList(QString path)
{
  if (m_path != path && !path.isEmpty())
    m_path = path;

  m_watch.removePaths(m_watch.directories());
  m_watch.addPath(m_path + "/" + m_folder);
  loadFolder(m_path);
}

void ImageList::dropEvent(QDropEvent* event)
{
  // Override to prevent list objects to be moved !
}

void ImageList::mouseMoveEvent(QMouseEvent* event)
{
  auto currentItem = this->currentItem();
  if (!currentItem)
    return;

  auto drag = new QDrag{this};
  auto mime = new QMimeData;
  mime->setText(m_id);
  mime->setUrls({QUrl{m_folder + currentItem->text()}});

  QRect zone = visualItemRect(currentItem);
  QPixmap pixmap(zone.size());
  viewport()->render(&pixmap, QPoint(), QRegion(zone));
  drag->setPixmap(pixmap);

  drag->setHotSpot(event->pos() - visualItemRect(currentItem).topLeft());
  drag->setMimeData(mime);
  drag->exec();
}

void ImageList::addFile(const QFileInfo& fileInfo)
{
  if(fileInfo.suffix() == "meta")
    return;
  auto item = new QListWidgetItem(fileInfo.fileName());

  QTimer::singleShot(64, [=] {
    QIcon icon = ImageCache::instance().inspector(fileInfo);

    auto items
        = this->findItems(fileInfo.fileName(), Qt::MatchFlag::MatchExactly);

    for (auto item : items)
      item->setIcon(icon);
  });

  this->addItem(item);
}

void ImageList::loadFolder(const QString& path)
{
  this->clear();
  QDir imageDirectory(m_path + "/" + m_folder);
  if (imageDirectory.exists())
  {
    imageDirectory.setFilter(QDir::Files);
    imageDirectory.setSorting(QDir::Name);

    auto fileList = imageDirectory.entryInfoList();

    for (int i = 0; i < fileList.size(); ++i)
    {
      addFile(fileList.at(i));
    }
  }
}

} // namespace SEGMent

namespace SEGMent
{
QMimeData* SoundModel::mimeData(const QModelIndexList& indexes) const
{
  if (!indexes.empty())
  {
    auto mime = new QMimeData;
    mime->setText(SEGMENT_SOUND_ID);

    auto path = filePath(indexes.first());
    path.remove(ressourcesPath);
    path.remove(0, 1);

    mime->setUrls({QUrl{path}});

    return mime;
  }
  return nullptr;
}

SoundList::SoundList(QString path, QString folder, QString id)
    : m_path{path}, m_folder{folder}, m_id{id}
{
  this->setDragEnabled(true);
  this->setModel(&m_model);
  this->setColumnWidth(0, 200);
  connect(this, &SoundList::doubleClicked, this, [=](const auto& idx) {
    auto& sp = SoundPlayer::instance();
    sp.play(QUrl::fromLocalFile(m_model.filePath(idx)));
  });
}

void SoundList::reloadList(QString path)
{
  if (m_path != path && !path.isEmpty())
    m_path = path;

  setModel(&m_model);
  m_model.ressourcesPath = m_path;
  this->setRootIndex(m_model.setRootPath(m_path + "/" + m_folder));
}

void SoundList::clear()
{
  setModel(nullptr);
}

} // namespace SEGMent

namespace SEGMent
{
TemplateList::TemplateList(QString resPath, QString folder, QString id)
    : m_path{resPath}, m_folder{folder}, m_id{id}
{
  connect(
      &m_watch,
      &QFileSystemWatcher::directoryChanged,
      this,
      &TemplateList::loadFolder);
  auto shortcut = new QShortcut{QKeySequence::Delete, this};
  shortcut->setContext(Qt::WidgetShortcut);
  connect(shortcut, &QShortcut::activated, this, [=] {
    auto currentItem = this->currentItem();
    if (!currentItem)
      return;
    auto file = m_path + "/" + m_folder + currentItem->text();
    if (QFile f{file}; f.exists())
      f.remove();
  });
}

void TemplateList::startDrag(Qt::DropActions supportedActions)
{
  QListWidget::startDrag(supportedActions);
}

void TemplateList::dragEnterEvent(QDragEnterEvent* event) {}

void TemplateList::reloadList(QString path)
{
  if (m_path != path && !path.isEmpty())
    m_path = path;

  m_watch.removePaths(m_watch.directories());
  m_watch.addPath(m_path + "/" + m_folder);
  loadFolder(m_path);
}

void TemplateList::dropEvent(QDropEvent* event) {}

void TemplateList::mouseMoveEvent(QMouseEvent* event)
{
  auto currentItem = this->currentItem();
  if (!currentItem)
    return;

  auto drag = new QDrag{this};
  auto mime = new QMimeData;
  mime->setText(m_id);
  mime->setUrls({QUrl{m_folder + currentItem->text()}});

  QRect zone = visualItemRect(currentItem);
  QPixmap pixmap(zone.size());
  viewport()->render(&pixmap, QPoint(), QRegion(zone));
  drag->setPixmap(pixmap);

  drag->setHotSpot(event->pos() - visualItemRect(currentItem).topLeft());
  drag->setMimeData(mime);
  drag->exec();
}

void TemplateList::loadFolder(const QString& folder)
{
  this->clear();
  QDir dir(m_path + "/" + m_folder);
  if (dir.exists())
  {
    dir.setFilter(QDir::Files);
    dir.setSorting(QDir::Name);

    auto fileList = dir.entryInfoList();

    for (int i = 0; i < fileList.size(); ++i)
    {
      auto fileInfo = fileList.at(i);
      if (fileInfo.suffix().contains("template"))
      {
        if (auto file = QFile{fileInfo.absoluteFilePath()};
            file.open(QIODevice::ReadOnly))
        {
          auto data = file.readAll();
          QJsonParseError err{};
          auto json = QJsonDocument::fromJson(data, &err);
          if (err.error == QJsonParseError::NoError)
          {
            auto pixmap = QByteArray::fromBase64(
                json.object()["Pixmap"].toString().toLatin1(),
                QByteArray::Base64Encoding);

            QImage img = QImage::fromData(pixmap, "PNG");
            QIcon icon{QPixmap::fromImage(img)};
            this->addItem(new QListWidgetItem(icon, fileInfo.fileName()));
          }
        }
      }
    }
  }
}

} // namespace SEGMent
