#pragma once
#include <score/model/Entity.hpp>
#include <score/model/path/PathSerialization.hpp>
#include <score/selection/Selectable.hpp>

#include <SEGMent/Items/Anchor.hpp>
#include <SEGMent/Model/ClickArea.hpp>
#include <SEGMent/Model/GifObject.hpp>
#include <SEGMent/Model/Model.hpp>
#include <SEGMent/Model/Riddle.hpp>
#include <SEGMent/Model/Scene.hpp>
#include <SEGMent/Model/ImageModel.hpp>
#include <SEGMent/Model/TransitionData.hpp>

namespace SEGMent
{

/**
 * @brief Class of the segment data model which carries the transition information,
 * as well as a couple of common properties across transitions.
 */
class TransitionModel : public score::Entity<TransitionModel>
{
public:
  W_OBJECT(TransitionModel)
  SCORE_SERIALIZE_FRIENDS

public:
  Selectable selection{this};

  TransitionModel(
      transition_t,
      const Id<TransitionModel>& id,
      QObject* parent);

  template <typename DeserializerVisitor>
  TransitionModel(DeserializerVisitor&& vis, QObject* parent)
      : Entity{vis, parent}
  {
    vis.writeTo(*this);
  }

  enum FadeMode
  {
    Immediate,
    Crossfade
  };

public:
  const transition_t& transition() const MSVC_NOEXCEPT;
  void setTransition(const transition_t& v) MSVC_NOEXCEPT;
  void transitionChanged(const transition_t& v) W_SIGNAL(transitionChanged, v);
  PROPERTY(
      transition_t,
      transition READ transition WRITE setTransition NOTIFY transitionChanged)
private:
  transition_t m_transition{};

public:
  const FadeMode& fade() const MSVC_NOEXCEPT;
  void setFade(const FadeMode& v) MSVC_NOEXCEPT;
  void fadeChanged(const FadeMode& v) W_SIGNAL(fadeChanged, v);
  PROPERTY(FadeMode, fade READ fade WRITE setFade NOTIFY fadeChanged)
private:
  FadeMode m_fade{FadeMode::Crossfade};

public:
  QColor color() const MSVC_NOEXCEPT;
  void setColor(QColor v) MSVC_NOEXCEPT;
  void colorChanged(QColor v) W_SIGNAL(colorChanged, v);
  PROPERTY(QColor, color READ color WRITE setColor NOTIFY colorChanged)
private:
  QColor m_color{};

public:
  const Sound& sound() const MSVC_NOEXCEPT;
  Sound& sound_ref() MSVC_NOEXCEPT;
  void setSound(const Sound& v) MSVC_NOEXCEPT;
  void soundChanged(const Sound& v) W_SIGNAL(soundChanged, v);
  PROPERTY(Sound, sound READ sound WRITE setSound NOTIFY soundChanged)
private:
  Sound m_sound{};

public:
  bool unique() const MSVC_NOEXCEPT;
  void setUnique(bool v) MSVC_NOEXCEPT;
  void uniqueChanged(bool v) W_SIGNAL(uniqueChanged, v);
  PROPERTY(bool, unique READ unique WRITE setUnique NOTIFY uniqueChanged)
private:
  bool m_unique{};

public:
  const QString& eventToAdd() const MSVC_NOEXCEPT;
  void setEventToAdd(const QString& v) MSVC_NOEXCEPT;
  void eventToAddChanged(const QString& v) W_SIGNAL(eventToAddChanged, v);
  PROPERTY(QString, eventToAdd READ eventToAdd WRITE setEventToAdd NOTIFY eventToAddChanged)
private:
  QString m_eventToAdd{};

public:
  const QString& eventToRemove() const MSVC_NOEXCEPT;
  void setEventToRemove(const QString& v) MSVC_NOEXCEPT;
  void eventToRemoveChanged(const QString& v) W_SIGNAL(eventToRemoveChanged, v);
  PROPERTY(QString, eventToRemove READ eventToRemove WRITE setEventToRemove NOTIFY eventToRemoveChanged)
private:
  QString m_eventToRemove{};

public:
  const QString& eventRequired() const MSVC_NOEXCEPT;
  void setEventRequired(const QString& v) MSVC_NOEXCEPT;
  void eventRequiredChanged(const QString& v) W_SIGNAL(eventRequiredChanged, v);
  PROPERTY(QString, eventRequired READ eventRequired WRITE setEventRequired NOTIFY eventRequiredChanged)
private:
  QString m_eventRequired{};

public:
  const QString& video() const MSVC_NOEXCEPT;
  void setVideo(const QString& v) MSVC_NOEXCEPT;
  void videoChanged(const QString& v) W_SIGNAL(videoChanged, v);
  PROPERTY(QString, video READ video WRITE setVideo NOTIFY videoChanged)
private:
  QString m_video{};

public:
  bool videoEachTime() const MSVC_NOEXCEPT;
  void setVideoEachTime(bool v) MSVC_NOEXCEPT;
  void videoEachTimeChanged(bool v) W_SIGNAL(videoEachTimeChanged, v);
  PROPERTY(bool, videoEachTime READ videoEachTime WRITE setVideoEachTime NOTIFY videoEachTimeChanged)
private:
  bool m_videoEachTime{};
};

} // namespace SEGMent


Q_DECLARE_METATYPE(SEGMent::TransitionModel::FadeMode)
W_REGISTER_ARGTYPE(SEGMent::TransitionModel::FadeMode)

DEFAULT_MODEL_METADATA(SEGMent::TransitionModel, "Transition")
