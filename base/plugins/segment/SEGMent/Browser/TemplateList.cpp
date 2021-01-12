#include <SEGMent/Browser/TemplateList.hpp>
#include <QShortcut>
#include <QDrag>
#include <QFile>
#include <QMimeData>
#include <QMouseEvent>
#include <QDir>
#include <QJsonDocument>

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
