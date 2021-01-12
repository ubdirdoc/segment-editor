#pragma once
#include <SEGMent/Model/Sound.hpp>
#include <SEGMent/FilePath.hpp>

#include <QMediaPlayer>
#include <QUrl>

namespace SEGMent
{
/**
 * @brief A simple sound player, used in the inspectors
 * to play the Sound instances associated to SEGMent objects.
 */
struct SoundPlayer
{
  QMediaPlayer player;

  static SoundPlayer& instance()
  {
    static SoundPlayer p;
    return p;
  }

  static auto convertVolume(double vol)
  {
    return QAudio::convertVolume(
               vol / qreal(100.0),
               QAudio::LogarithmicVolumeScale,
               QAudio::LinearVolumeScale)
           * 100.;
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

  void setVolume(int vol) { player.setVolume(convertVolume(vol)); }

  void stop() { player.stop(); }
};

}
