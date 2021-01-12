#include <SEGMent/Browser/ImageList.hpp>
#include <SEGMent/ImageCache.hpp>

#include <QMouseEvent>
#include <QUrl>
#include <QMimeData>
#include <QDrag>
#include <QTimer>

namespace SEGMent
{
ImageList::ImageList(QString resPath, QString folder, QString id)
    : m_path{resPath}, m_folder{folder}, m_id{id}
{
  setAcceptDrops(true);
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

void ImageList::dragEnterEvent(QDragEnterEvent* event)
{
  event->accept();
}
void ImageList::dragMoveEvent(QDragMoveEvent* event)
{
  event->accept();
}
void ImageList::dragLeaveEvent(QDragLeaveEvent* event)
{
  event->accept();
}

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
  for(const QUrl& file : event->mimeData()->urls())
  {
    auto path = file.toLocalFile();
    QImage img{path};
    if(!img.isNull())
    {
      QFile::copy(path, this->m_path + "/" + m_folder + QFileInfo{path}.fileName());
    }
  }
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

  QTimer::singleShot(64, this, [=] {
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
