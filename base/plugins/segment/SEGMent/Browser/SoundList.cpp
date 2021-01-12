#include <SEGMent/Browser/SoundList.hpp>
#include <SEGMent/Items/GlobalVariables.hpp>
#include <SEGMent/SoundPlayer.hpp>

#include <QMimeData>
#include <QDrag>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDragLeaveEvent>

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
  this->setAcceptDrops(true);
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

void SoundList::dragEnterEvent(QDragEnterEvent* event)
{
  event->accept();
}
void SoundList::dragMoveEvent(QDragMoveEvent* event)
{
  event->accept();
}
void SoundList::dragLeaveEvent(QDragLeaveEvent* event)
{
  event->accept();
}
void SoundList::dropEvent(QDropEvent* event)
{
  for(const QUrl& file : event->mimeData()->urls())
  {
    auto path = file.toLocalFile();
    QFileInfo p{path};
    if(p.completeSuffix() == "wav")
    {
      QFile::copy(path, this->m_path + "/" + m_folder + p.fileName());
    }
  }
}

void SoundList::clear()
{
  setModel(nullptr);
}

} // namespace SEGMent
