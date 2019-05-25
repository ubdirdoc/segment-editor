#pragma once
#include <score/model/Entity.hpp>
#include <score/selection/Selectable.hpp>
#include <SEGMent/FilePath.hpp>

#include <QMediaPlayer>
#if defined(Q_CC_MSVC)
#define MSVC_NOEXCEPT
#else
#define MSVC_NOEXCEPT noexcept
#endif
namespace SEGMent
{

class Sound : public QObject
{
  W_OBJECT(Sound)
  SCORE_SERIALIZE_FRIENDS
public:
  Sound() = default;
  Sound(const Sound&);
  Sound(Sound&&);
  Sound& operator=(const Sound&);
  Sound& operator=(Sound&&);

  Sound(const QString& s): m_path{s} { }

  friend bool operator==(const Sound& lhs, const Sound& rhs);
  friend bool operator!=(const Sound& lhs, const Sound& rhs);

public:
  const QString& path() const MSVC_NOEXCEPT;
  void setPath(const QString& v) MSVC_NOEXCEPT;
  void pathChanged(const QString& v) W_SIGNAL(pathChanged, v);
  PROPERTY(QString, path READ path WRITE setPath NOTIFY pathChanged)
private:
  QString m_path;

public:
  const double& volume() const MSVC_NOEXCEPT;
  void setVolume(const double& v) MSVC_NOEXCEPT;
  void volumeChanged(const double& v) W_SIGNAL(volumeChanged, v);
  PROPERTY(double, volume READ volume WRITE setVolume NOTIFY volumeChanged)
private:
  double m_volume{0.7};

public:
  const double& range() const MSVC_NOEXCEPT;
  void setRange(const double& v) MSVC_NOEXCEPT;
  void rangeChanged(const double& v) W_SIGNAL(rangeChanged, v);
  PROPERTY(double, range READ range WRITE setRange NOTIFY rangeChanged)
private:
  double m_range{};

public:
  bool repeat() const MSVC_NOEXCEPT;
  void setRepeat(bool v) MSVC_NOEXCEPT;
  void repeatChanged(bool v) W_SIGNAL(repeatChanged, v);
  PROPERTY(bool, repeat READ repeat WRITE setRepeat NOTIFY repeatChanged)
private:
      bool m_repeat{true};
};

struct SoundPlayer
{
  QMediaPlayer player;

  static SoundPlayer& instance()
  { static SoundPlayer p; return p; }

  static auto convertVolume(double vol)
  {
    return QAudio::convertVolume(vol / qreal(100.0), QAudio::LogarithmicVolumeScale, QAudio::LinearVolumeScale) * 100.;
  }
  void play(const Sound& snd, const score::DocumentContext& ctx)
  {
    player.setMedia(QUrl::fromLocalFile(toLocalFile(snd.path(), ctx)));
    player.setVolume(convertVolume(snd.volume() * 100.));
    player.play();
  }

  void play(const QUrl& snd)
  {
    player.setMedia(snd);
    player.setVolume(75);
    player.play();
  }

  void setVolume(int vol)
  {
    player.setVolume(convertVolume(vol));
  }

  void stop()
  {
    player.stop();
  }
};

}

W_REGISTER_ARGTYPE(SEGMent::Sound)
