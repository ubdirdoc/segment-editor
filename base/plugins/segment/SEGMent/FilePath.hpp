#pragma once
#include <score/document/DocumentContext.hpp>

#include <core/document/Document.hpp>

#include <QApplication>
#include <QDir>
#include <QFileInfo>
#include <QString>
#include <QUrl>

namespace SEGMent
{

inline QString
toLocalFile(const QString& filename, const score::DocumentContext& ctx)
{
  if (QFileInfo(filename).exists())
    return filename;

  auto docroot = ctx.document.metadata().fileName();
  if (QFileInfo rt(docroot); rt.exists())
  {
    docroot = rt.absolutePath();
    if (QString p(docroot + "/" + filename); QFileInfo(p).exists())
      return p;
    if (QString p(docroot + "/Ressources/" + filename); QFileInfo(p).exists())
      return p;
  }

  auto approot = qApp->applicationDirPath();
  if (QString p(approot + "/" + filename); QFileInfo(p).exists())
    return p;
  if (QString p(approot + "/Ressources/" + filename); QFileInfo(p).exists())
    return p;

  return filename;
}

inline QString ressourcesFolder(const score::DocumentContext& ctx)
{
  auto docroot = ctx.document.metadata().fileName();

  if (QFileInfo f(docroot + "/Ressources/"); f.exists())
    return f.absolutePath();
  if (QFileInfo f(docroot); f.exists())
    return f.absolutePath();

  auto approot = qApp->applicationDirPath();
  if (QFileInfo(approot).exists())
    return approot + "/Ressources/";

  return QString{"Ressources/"};
}

inline QString
toLocalFile(const QUrl& filename, const score::DocumentContext& ctx)
{
  return toLocalFile(filename.toLocalFile(), ctx);
}

// Taken from https://stackoverflow.com/a/18073392/1495627
// Adds a unique suffix to a file name so no existing file has the same file
// name. Can be used to avoid overwriting existing files. Works for both
// files/directories, and both relative/absolute paths. The suffix is in the
// form - "path/to/file.tar.gz", "path/to/file (1).tar.gz",
// "path/to/file (2).tar.gz", etc.
inline QString addUniqueSuffix(const QString& fileName)
{
  // If the file doesn't exist return the same name.
  if (!QFile::exists(fileName))
  {
    return fileName;
  }

  QFileInfo fileInfo(fileName);
  QString ret;

  // Split the file into 2 parts - dot+extension, and everything else. For
  // example, "path/file.tar.gz" becomes "path/file"+".tar.gz", while
  // "path/file" (note lack of extension) becomes "path/file"+"".
  QString secondPart = fileInfo.completeSuffix();
  QString firstPart;
  if (!secondPart.isEmpty())
  {
    secondPart = "." + secondPart;
    firstPart = fileName.left(fileName.size() - secondPart.size());
  }
  else
  {
    firstPart = fileName;
  }

  // Try with an ever-increasing number suffix, until we've reached a file
  // that does not yet exist.
  for (int ii = 1;; ii++)
  {
    // Construct the new file name by adding the unique number between the
    // first and second part.
    ret = QString("%1 (%2)%3").arg(firstPart).arg(ii).arg(secondPart);
    // If no file exists with the new name, return it.
    if (!QFile::exists(ret))
    {
      return ret;
    }
  }
}

// Taken from https://forum.qt.io/topic/59245/is-there-any-api-to-recursively-copy-a-directory-and-all-it-s-sub-dirs-and-files/4
// Small change : won't overwrite destination file if it already exists
inline bool copyRecursively(QString sourceFolder, QString destFolder)
{
  bool success = false;
  QDir sourceDir(sourceFolder);

  if(!sourceDir.exists())
    return false;

  QDir destDir(destFolder);
  if(!destDir.exists())
    destDir.mkdir(destFolder);

  QStringList files = sourceDir.entryList(QDir::Files);
  for(int i = 0; i< files.count(); i++) {
    QString srcName = sourceFolder + QDir::separator() + files[i];
    QString destName = destFolder + QDir::separator() + files[i];
    if(!QFileInfo{destName}.exists())
    {
      success = QFile::copy(srcName, destName);
      if(!success)
        return false;
    }
  }

  files.clear();
  files = sourceDir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot);
  for(int i = 0; i< files.count(); i++)
  {
    QString srcName = sourceFolder + QDir::separator() + files[i];
    QString destName = destFolder + QDir::separator() + files[i];
    success = copyRecursively(srcName, destName);
    if(!success)
      return false;
  }

  return true;
}
} // namespace SEGMent
