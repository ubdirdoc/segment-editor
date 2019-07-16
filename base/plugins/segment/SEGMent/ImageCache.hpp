#pragma once
#define NOMINMAX 1
#define WIN32_MEAN_AND_LEAN 1
#include <QString>
#include <QFileInfo>
#include <QDateTime>
#include <QPixmap>
#include <QHash>
#include <QSettings>
#include <QDir>
#include <QDebug>
#include <QStandardPaths>
#include <QApplication>
#include <QCryptographicHash>
#include <QMessageBox>
#include <QPainter>
#include <QGraphicsPixmapItem>
#include <QStyleOption>
#include <QImageWriter>
#include <score/tools/std/StringHash.hpp>
#undef small

namespace SEGMent
{
class ImageCache;
struct CacheInstance
{
public:
  qint64 lastChangeTimestamp{};
  QString inspector;
  QString small;
  QString large;
  QString full;
  mutable QSize full_size;

  static const QPixmap& missingImage() noexcept
  {
    static const QPixmap m{[] {
      QPixmap img(640, 480);
      img.fill();
      {
        QPainter p{&img};
        p.setPen(QPen(Qt::red, 3));
        p.drawLine(0, 0, 640, 480);
        p.drawLine(640, 0, 0, 480);
      }

      return img;
    }()};
    return m;
  }
  const QPixmap& smallPixmap() const
  {
      if(small_pixmap.isNull())
      {
          small_pixmap = small;
          if(small_pixmap.isNull())
          {
              small_pixmap = missingImage();
              full_size = {640, 480};
          }
      }
      return small_pixmap;
  }
  const QPixmap& largePixmap() const
  {
      if(large_pixmap.isNull())
      {
          large_pixmap = large;
          if(large_pixmap.isNull())
          {
              large_pixmap = missingImage();
              full_size = {640, 480};
          }
      }
      return large_pixmap;
  }
  const QPixmap& fullPixmap() const
  {
      if(full_pixmap.isNull())
      {
          full_pixmap = full;
          full_size = full_pixmap.size();
          if(full_pixmap.isNull())
          {
              full_pixmap = missingImage();
              full_size = {640, 480};
          }
      }
      return full_pixmap;
  }
  const QPixmap& inspectorPixmap() const
  {
      if(inspector_pixmap.isNull())
      {
          inspector_pixmap = inspector;
          if(inspector_pixmap.isNull())
          {
              inspector_pixmap = missingImage();
              full_size = {640, 480};
          }
      }
      return inspector_pixmap;
  }

  friend QDataStream& operator<<(QDataStream& s, const CacheInstance& i)
  {
    return s << i.lastChangeTimestamp << i.inspector << i.small << i.large << i.full << i.full_size;
  }

  friend QDebug& operator<<(QDebug& s, const CacheInstance& i)
  {
    return s << i.lastChangeTimestamp << i.inspector << i.small << i.large << i.full;
  }

  friend QDataStream& operator>>(QDataStream& s, CacheInstance& i)
  {
    return s >> i.lastChangeTimestamp >> i.inspector >> i.small >> i.large >> i.full  >> i.full_size;
  }

private:
    friend class ImageCache;
    mutable QPixmap inspector_pixmap;
    mutable QPixmap small_pixmap;
    mutable QPixmap large_pixmap;
    mutable QPixmap full_pixmap;
};
}

inline QDataStream& operator<<(QDataStream& s, const std::unordered_map<QString, SEGMent::CacheInstance>& object)
{
  s << (int) object.size();
  for(auto it = object.begin(); it != object.end(); ++it)
  {
    s << (*it).first << (*it).second;
  }
  return s;
}

inline QDataStream& operator>>(QDataStream& s, std::unordered_map<QString, SEGMent::CacheInstance>& object)
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
Q_DECLARE_METATYPE_TEMPLATE_2ARG(std::unordered_map)

namespace SEGMent
{
class ImageCache
{
  using impl = std::unordered_map<QString, CacheInstance>;
public:
  static ImageCache& instance()
  {
    static ImageCache c;
    return c;
  }
  const QPixmap& inspector(const QFileInfo& path)
  {
    if(auto it = m_cache.find(path.absoluteFilePath());
       it != m_cache.end() &&
       path.lastModified().toSecsSinceEpoch() == it->second.lastChangeTimestamp &&
       QFileInfo(it->second.inspector).exists())
    {
        return it->second.inspectorPixmap();
    }
    else
    {
      return createCache(path).inspectorPixmap();
    }
  }

  const QPixmap& small(const QFileInfo& path)
  {
    if(auto it = m_cache.find(path.absoluteFilePath());
       it != m_cache.end() &&
       path.lastModified().toSecsSinceEpoch() == it->second.lastChangeTimestamp &&
       QFileInfo(it->second.small).exists())
    {
        return it->second.smallPixmap();
    }
    else
    {
      return createCache(path).smallPixmap();
    }
  }

  const QPixmap& large(const QFileInfo& path)
  {
    if(auto it = m_cache.find(path.absoluteFilePath());
       it != m_cache.end() &&
       path.lastModified().toSecsSinceEpoch() == it->second.lastChangeTimestamp &&
       QFileInfo(it->second.large).exists())
    {
      return it->second.largePixmap();
    }
    else
    {
      return createCache(path).largePixmap();
    }
  }

  const QPixmap& full(const QFileInfo& path)
  {
    if(auto it = m_cache.find(path.absoluteFilePath());
       it != m_cache.end() &&
       path.lastModified().toSecsSinceEpoch() == it->second.lastChangeTimestamp &&
       QFileInfo(it->second.full).exists())
    {
        return it->second.fullPixmap();
    }
    else
    {
      return createCache(path).fullPixmap();
    }
  }

  CacheInstance& cache(const QFileInfo& path)
  {
    if(auto it = m_cache.find(path.absoluteFilePath());
       it != m_cache.end() &&
       path.lastModified().toSecsSinceEpoch() == it->second.lastChangeTimestamp &&
       QFileInfo(it->second.full).exists() &&
       QFileInfo(it->second.large).exists() &&
       QFileInfo(it->second.small).exists() &&
       QFileInfo(it->second.inspector).exists()
     )
    {
      return it->second;
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

    c.full_size = c.full_pixmap.size();

    c.large_pixmap = c.full_pixmap.scaled(800 * dpi, 600 * dpi, Qt::KeepAspectRatio);
    c.small_pixmap = c.full_pixmap.scaled(160 * dpi, 120 * dpi, Qt::KeepAspectRatio);
    c.inspector_pixmap = c.full_pixmap.scaled(80 * dpi, 60 * dpi, Qt::KeepAspectRatio);

    // Gif : draw the camera icon in the inspector
    if (info.completeSuffix().compare("gif", Qt::CaseInsensitive) == 0)
    {
      QPainter p(&c.inspector_pixmap);
      p.setPen(Qt::black);
      p.setBrush(Qt::white);
      p.drawRect(4, 4, 24, 24);
      static const auto gif_icon = QPixmap(":/baseline-videocam-24px.svg");
      p.drawPixmap(4, 4, 24, 24, gif_icon);
    }

    auto loc = QStandardPaths::standardLocations(QStandardPaths::CacheLocation);
    if(!loc.isEmpty())
    {
      auto hash = path.toUtf8().toBase64(QByteArray::Base64Encoding | QByteArray::OmitTrailingEquals);
      auto large_path = loc.front() + QDir::separator() + hash + "_large.png";
      auto small_path = loc.front() + QDir::separator() + hash + "_small.png";
      auto insp_path = loc.front() + QDir::separator() + hash + "_insp.png";

      if(!QDir{loc.front()}.exists())
          QDir::root().mkpath(loc.front());

      c.large_pixmap.save(QDir::toNativeSeparators(large_path), "PNG", 60);
      c.small_pixmap.save(QDir::toNativeSeparators(small_path), "PNG", 60);
      c.inspector_pixmap.save(QDir::toNativeSeparators(insp_path), "PNG", 60);

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
    this->cache = &cache;
    if(cache.full_size.width() < 1)
        cache.fullPixmap();
    update();
  }
  QRectF boundingRect() const override
  {
    if(cache) {
        return {0, 0, (qreal)cache->full_size.width(), (qreal)cache->full_size.height()};
    }else
      return {};
  }

  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override
  {
    const auto lod = option->levelOfDetailFromTransform(painter->worldTransform());

    if(cache)
    {
      const int fw = cache->full_size.width();
      const int fh = cache->full_size.height();
      const auto viewWidth = lod * fw;
      const auto rect = QRect{0, 0, fw, fh};

      painter->drawPixmap(rect, cache->fullPixmap());
      return;
      if(viewWidth < 100)
        painter->drawPixmap(rect, cache->inspectorPixmap());
      if(viewWidth < 300)
          painter->drawPixmap(rect, cache->smallPixmap());
      else if(viewWidth < 800)
          painter->drawPixmap(rect, cache->largePixmap());
      else
          painter->drawPixmap(rect, cache->fullPixmap());
    }
  }

private:
  const CacheInstance* cache{};
};
}
