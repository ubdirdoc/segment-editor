#pragma once
#include <QListWidget>
#include <QFileInfo>
#include <QFileSystemWatcher>

namespace SEGMent
{

//! This widget displays the images in a folder.
//! The id is passed as text of the mime data when dragging an image.
//! It is used in the inspector.
class ImageList : public QListWidget
{
public:
  explicit ImageList(QString resPath, QString folder, QString id);
  void reloadList(QString resPath = QString{});

private:
  void startDrag(Qt::DropActions supportedActions) override;
  void dropEvent(QDropEvent* event) override;
  void dragEnterEvent(QDragEnterEvent* event) override;
  void dragMoveEvent(QDragMoveEvent* event) override;
  void dragLeaveEvent(QDragLeaveEvent* event) override;

  void mouseMoveEvent(QMouseEvent* event) override;

  void addFile(const QFileInfo&);
  void removeFile(const QFileInfo&);

  void loadFolder(const QString& folder);

  QFileSystemWatcher m_watch;
  QString m_path;
  QString m_folder;
  QString m_id;
};

}
