#pragma once
#include <QFileSystemModel>
#include <QFileSystemWatcher>
#include <QListWidget>
#include <QTreeView>

namespace SEGMent
{
//! This widget displays the images in a folder.
//!  The id is passed as text of the mime data when dragging an image.
class ImageList : public QListWidget
{
public:
  explicit ImageList(QString resPath, QString folder, QString id);
  void reloadList(QString resPath = QString{});

private:
  void startDrag(Qt::DropActions supportedActions) override;
  void dropEvent(QDropEvent* event) override;
  void dragEnterEvent(QDragEnterEvent* event) override;

  void mouseMoveEvent(QMouseEvent* event) override;

  void addFile(const QFileInfo&);
  void removeFile(const QFileInfo&);

  void loadFolder(const QString& folder);

  QFileSystemWatcher m_watch;
  QString m_path;
  QString m_folder;
  QString m_id;
};

class SoundModel final : public QFileSystemModel
{
public:
  using QFileSystemModel::QFileSystemModel;
  QString ressourcesPath;

private:
  QMimeData* mimeData(const QModelIndexList& indexes) const override;
};

class SoundList : public QTreeView
{
public:
  explicit SoundList(QString path, QString folder, QString id);
  void reloadList(QString path = QString{});
  void clear();

  QString m_path;
  QString m_folder;
  QString m_id;
  SoundModel m_model;
};

class TemplateList final : public QListWidget
{
public:
  explicit TemplateList(QString resPath, QString folder, QString id);
  void reloadList(QString resPath = QString{});

private:
  void startDrag(Qt::DropActions supportedActions) override;
  void dropEvent(QDropEvent* event) override;
  void dragEnterEvent(QDragEnterEvent* event) override;

  void mouseMoveEvent(QMouseEvent* event) override;

  void loadFolder(const QString& folder);

  QFileSystemWatcher m_watch;
  QString m_path;
  QString m_folder;
  QString m_id;
};
} // namespace SEGMent
