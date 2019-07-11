#pragma once
#include <QString>
#include <QFileInfo>
#include <QDateTime>
#include <QPixmap>
#include <QHash>
#include <QSettings>
#include <QDir>
#include <QStandardPaths>
#include <QApplication>
#include <QCryptographicHash>
#include <QPainter>
#include <QGraphicsPixmapItem>
#include <QStyleOption>
namespace SEGMent
{

struct CacheInstance
{
  qint64 lastChangeTimestamp{};
  QString inspector;
  QString small;
  QString large;
  QString full;
  QPixmap small_pixmap;
  QPixmap large_pixmap;
  QPixmap full_pixmap;

  friend QDataStream& operator<<(QDataStream& s, const CacheInstance& i)
  {
    return s << i.lastChangeTimestamp << i.inspector << i.small << i.large << i.full;
  }

  friend QDebug& operator<<(QDebug& s, const CacheInstance& i)
  {
    return s << i.lastChangeTimestamp << i.inspector << i.small << i.large << i.full;
  }

  friend QDataStream& operator>>(QDataStream& s, CacheInstance& i)
  {
    return s >> i.lastChangeTimestamp >> i.inspector >> i.small >> i.large >> i.full;
  }
};
}

inline QDataStream& operator<<(QDataStream& s, const QHash<QString, SEGMent::CacheInstance>& object)
{
  s << (int) object.size();
  for(auto it = object.keyValueBegin(); it != object.keyValueEnd(); ++it)
  {
    s << (*it).first << (*it).second;
  }
  return s;
}

inline QDataStream& operator>>(QDataStream& s, QHash<QString, SEGMent::CacheInstance>& object)
{
  int sz = 0;
  s >> sz;
  for(int j = 0; j < sz; j++)
  {
    QString k;
    SEGMent::CacheInstance v;
    s >> k >> v;
    object[std::move(k)] = std::move(v);
  }
  return s;
}

Q_DECLARE_METATYPE(SEGMent::CacheInstance)

namespace SEGMent
{
class ImageCache
{
  using impl = QHash<QString, CacheInstance>;
public:
  static ImageCache& instance()
  {
    static ImageCache c;
    return c;
  }
  QPixmap inspector(const QFileInfo& path)
  {
    if(auto it = m_cache.find(path.absoluteFilePath());
       it != m_cache.end() && path.lastModified().toSecsSinceEpoch() == it->lastChangeTimestamp)
    {
      return it->inspector;
    }
    else
    {
      return createCache(path).inspector;
    }
  }

  const QPixmap& small(const QFileInfo& path)
  {
    if(auto it = m_cache.find(path.absoluteFilePath());
       it != m_cache.end() && path.lastModified().toSecsSinceEpoch() == it->lastChangeTimestamp)
    {
      if(it->small_pixmap.isNull())
        it->small_pixmap = it->small;
      return it->small_pixmap;
    }
    else
    {
      return createCache(path).small_pixmap;
    }
  }

  const QPixmap& large(const QFileInfo& path)
  {
    if(auto it = m_cache.find(path.absoluteFilePath());
       it != m_cache.end() && path.lastModified().toSecsSinceEpoch() == it->lastChangeTimestamp)
    {
      if(it->large_pixmap.isNull())
        it->large_pixmap = it->large;
      return it->large_pixmap;
    }
    else
    {
      return createCache(path).large_pixmap;
    }
  }

  const QPixmap& full(const QFileInfo& path)
  {
    if(auto it = m_cache.find(path.absoluteFilePath());
       it != m_cache.end() && path.lastModified().toSecsSinceEpoch() == it->lastChangeTimestamp)
    {
      if(it->full_pixmap.isNull())
        it->full_pixmap = it->full;
      return it->full_pixmap;
    }
    else
    {
      return createCache(path).full_pixmap;
    }
  }

  CacheInstance& cache(const QFileInfo& path)
  {
    if(auto it = m_cache.find(path.absoluteFilePath());
       it != m_cache.end() && path.lastModified().toSecsSinceEpoch() == it->lastChangeTimestamp)
    {
      if(it->full_pixmap.isNull())
        it->full_pixmap = it->full;
      if(it->large_pixmap.isNull())
        it->large_pixmap = it->large;
      if(it->small_pixmap.isNull())
        it->small_pixmap = it->small;
      return *it;
    }
    else
    {
      return createCache(path);
    }
  }

private:
  CacheInstance& createCache(const QFileInfo& info)
  {
    const auto& path = info.absoluteFilePath();
    auto& c = m_cache[path];

    auto dpi = qApp->devicePixelRatio();
    c.full_pixmap = QPixmap(path);
    if(c.full_pixmap.isNull())
      return c;

    c.large_pixmap = c.full_pixmap.scaled(800 * dpi, 600 * dpi, Qt::KeepAspectRatio);
    c.small_pixmap = c.full_pixmap.scaled(160 * dpi, 120 * dpi, Qt::KeepAspectRatio);
    auto insp = c.full_pixmap.scaled(80 * dpi, 60 * dpi, Qt::KeepAspectRatio);

    // Gif : draw the camera icon in the inspector
    if (info.completeSuffix().compare("gif", Qt::CaseInsensitive) == 0)
    {
      QPainter p(&insp);
      p.setPen(Qt::black);
      p.setBrush(Qt::white);
      p.drawRect(4, 4, 24, 24);
      p.drawPixmap(4, 4, 24, 24, QPixmap(":/baseline-videocam-24px.svg"));
    }

    auto loc = QStandardPaths::standardLocations(QStandardPaths::CacheLocation);
    if(!loc.isEmpty())
    {
      auto hash = path.toUtf8().toBase64(QByteArray::Base64Encoding | QByteArray::OmitTrailingEquals);
      auto large_path = loc.front() + QDir::separator() + hash + "_large.jpeg";
      auto small_path = loc.front() + QDir::separator() + hash + "_small.jpeg";
      auto insp_path = loc.front() + QDir::separator() + hash + "_insp.jpeg";
      c.large_pixmap.save(large_path, "JPG", 60);
      c.small_pixmap.save(small_path, "JPG", 60);
      insp.save(insp_path, "JPG", 60);

      c.lastChangeTimestamp = info.lastModified().toSecsSinceEpoch();
      c.large = large_path;
      c.small = small_path;
      c.inspector = insp_path;
      c.full = path;
    }
    return c;
  }

  impl m_cache;
  ImageCache()
  {
    QSettings s;
    m_cache = s.value("editor/imagecache").value<impl>();
  }

  ~ImageCache()
  {
    QSettings s;
    s.setValue("editor/imagecache", QVariant::fromValue(m_cache));
  }

  ImageCache(const ImageCache&) = default;
  ImageCache(ImageCache&&) = default;
  ImageCache& operator=(const ImageCache&) = default;
  ImageCache& operator=(ImageCache&&) = default;
};

class LODPixmapItem : public QGraphicsItem
{
public:
  using QGraphicsItem::QGraphicsItem;

  void setPixmap(const CacheInstance& cache)
  {
    prepareGeometryChange();
    small= &cache.small_pixmap;
    large = &cache.large_pixmap;
    full = &cache.full_pixmap;
    update();
  }
  QRectF boundingRect() const override
  {
    if(full)
      return {0, 0, (qreal)full->width(), (qreal)full->height()};
    else
      return {};
  }

  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override
  {
    const auto lod = option->levelOfDetailFromTransform(painter->worldTransform());
    if(full)
    {
      const auto rect = QRect{0, 0, full->width(), full->height()};
      if(lod < 0.3)
        painter->drawPixmap(rect, *small);
      else if(lod < 0.8)
        painter->drawPixmap(rect, *large);
      else
        painter->drawPixmap(rect, *full);
    }
  }

private:
  const QPixmap* small{};
  const QPixmap* large{};
  const QPixmap* full{};
};
}
