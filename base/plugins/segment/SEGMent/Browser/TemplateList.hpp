#pragma once
#include <QListWidget>
#include <QFileSystemWatcher>

namespace SEGMent
{

//! This widget displays the template files in a folder.
//! It is used in the inspector.
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
