#pragma once
#include <QFileSystemModel>
#include <QFileSystemWatcher>
#include <QTreeView>

namespace SEGMent
{

//! This contains the data model for the sound files available from the inspector.
class SoundModel final : public QFileSystemModel
{
public:
  using QFileSystemModel::QFileSystemModel;
  QString ressourcesPath;

private:
  QMimeData* mimeData(const QModelIndexList& indexes) const override;
};

//! This widget displays the sound files in a folder.
//! It is used in the inspector
class SoundList : public QTreeView
{
public:
  explicit SoundList(QString path, QString folder, QString id);
  void reloadList(QString path = QString{});
  void clear();

private:
  void dragEnterEvent(QDragEnterEvent* event) override;
  void dragMoveEvent(QDragMoveEvent* event) override;
  void dragLeaveEvent(QDragLeaveEvent* event) override;
  void dropEvent(QDropEvent* event);

  QString m_path;
  QString m_folder;
  QString m_id;
  SoundModel m_model;
};
}
