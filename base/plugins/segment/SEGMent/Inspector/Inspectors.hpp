#pragma once
#include <SEGMent/Inspector/InspectorSetup.hpp>

namespace SEGMent
{
//! Inspector for SceneModel
class SceneInspector : public QWidget
{
public:
  SceneInspector(
      const SceneModel& sc,
      const score::DocumentContext& doc,
      QWidget* parent)
      : QWidget{parent}
  {
    auto lay = new Inspector::Layout{this};
    setup_inspector(SceneModel::p_startText{}, sc, doc, *lay, this);
    setup_inspector(SceneModel::p_repeatText{}, sc, doc, *lay, this);
    setup_inspector(SceneModel::p_cue{}, sc, doc, *lay, this);
    setup_inspector(SceneModel::p_journal{}, sc, doc, *lay, this);
    setup_inspector(SceneModel::p_sonar{}, sc, doc, *lay, this);
    setup_inspector(SceneModel::p_ambience{}, sc, doc, *lay, this);
    setup_inspector(SceneModel::p_image{}, sc, doc, *lay, this);
    setup_inspector(SceneModel::p_sceneType{}, sc, doc, *lay, this);
  }
};

//! Inspector for ImageModel
class ImageInspector : public QWidget
{
public:
  ImageInspector(
      const ImageModel& sc,
      const score::DocumentContext& doc,
      QWidget* parent)
      : QWidget{parent}
  {
    auto lay = new Inspector::Layout{this};
    setup_inspector(ImageModel::p_sound{}, sc, doc, *lay, this);
    setup_inspector(ImageModel::p_image{}, sc, doc, *lay, this);
    setup_inspector(ImageModel::p_puzzle{}, sc, doc, *lay, this);
  }
};

//! Inspector for GifModel
class GifInspector : public QWidget
{
public:
  GifInspector(
      const GifModel& sc,
      const score::DocumentContext& doc,
      QWidget* parent)
      : QWidget{parent}
  {
    auto base_lay = new score::MarginLess<QVBoxLayout>{this};
    auto lay = new Inspector::Layout{this};
    setup_inspector(GifModel::p_sound{}, sc, doc, *lay, this);
    setup_inspector(GifModel::p_image{}, sc, doc, *lay, this);
    base_lay->addLayout(lay);

    auto f = WidgetFactory<GifModel::p_frames, GifModel>{sc, doc, *lay, this}
                 .make(sc.frames());
    base_lay->addWidget(f);
    // f->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding);

    QSizePolicy policy = f->sizePolicy();
    policy.setVerticalStretch(1);
    f->setSizePolicy(policy);
  }
};

//! Inspector for TransitionModel
class TransitionInspector : public QWidget
{
public:
  TransitionInspector(
      const TransitionModel& sc,
      const score::DocumentContext& doc,
      QWidget* parent)
      : QWidget{parent}
  {
    auto lay = new Inspector::Layout{this};
    setup_inspector(TransitionModel::p_sound{}, sc, doc, *lay, this);
    setup_inspector(TransitionModel::p_fade{}, sc, doc, *lay, this);
    // setup_inspector(TransitionModel::p_color{}, sc, doc, *lay, this);
    setup_inspector(TransitionModel::p_transition{}, sc, doc, *lay, this);
    setup_inspector(TransitionModel::p_unique{}, sc, doc, *lay, this);
    setup_inspector(TransitionModel::p_eventToAdd{}, sc, doc, *lay, this);
    setup_inspector(TransitionModel::p_eventToRemove{}, sc, doc, *lay, this);
    setup_inspector(TransitionModel::p_eventRequired{}, sc, doc, *lay, this);
  }
};

//! Inspector for ClickAreaModel
class ClickInspector : public QWidget
{
public:
  ClickInspector(
      const ClickAreaModel& sc,
      const score::DocumentContext& doc,
      QWidget* parent)
      : QWidget{parent}
  {
    auto lay = new Inspector::Layout{this};
    setup_inspector(ClickAreaModel::p_sound{}, sc, doc, *lay, this);
  }
};

//! Inspector for BackClickAreaModel
class BackClickInspector : public QWidget
{
public:
  BackClickInspector(
      const BackClickAreaModel& sc,
      const score::DocumentContext& doc,
      QWidget* parent)
      : QWidget{parent}
  {
    auto lay = new Inspector::Layout{this};
    setup_inspector(BackClickAreaModel::p_sound{}, sc, doc, *lay, this);
  }
};

//! Inspector for TextAreaModel
class TextInspector : public QWidget
{
public:
  TextInspector(
      const TextAreaModel& sc,
      const score::DocumentContext& doc,
      QWidget* parent)
      : QWidget{parent}
  {
    auto lay = new Inspector::Layout{this};
    setup_inspector(TextAreaModel::p_text{}, sc, doc, *lay, this);
    setup_inspector(TextAreaModel::p_behaviour{}, sc, doc, *lay, this);
    setup_inspector(TextAreaModel::p_sound{}, sc, doc, *lay, this);
  }
};

INSPECTOR_FACTORY(
    ImageInspectorFactory,
    ImageModel,
    ImageInspector,
    "e3a3d882-549d-46ef-9c2c-336e4893d7b6")
INSPECTOR_FACTORY(
    GifInspectorFactory,
    GifModel,
    GifInspector,
    "4e379cda-508e-4999-b9da-e76f55cb0e28")
INSPECTOR_FACTORY(
    SceneInspectorFactory,
    SceneModel,
    SceneInspector,
    "1ca16c0a-6c01-4054-a646-d860a3886e81")
INSPECTOR_FACTORY(
    TransitionInspectorFactory,
    TransitionModel,
    TransitionInspector,
    "48e36531-9477-4d77-bdc3-b5594c06b505")
INSPECTOR_FACTORY(
    ClickInspectorFactory,
    ClickAreaModel,
    ClickInspector,
    "556bf888-3e25-4b2d-a5a3-1b6b987cb1f7")
INSPECTOR_FACTORY(
    BackClickInspectorFactory,
    BackClickAreaModel,
    BackClickInspector,
    "8522cee8-08b8-4055-82fa-560f8ddfca5f")
INSPECTOR_FACTORY(
    TextInspectorFactory,
    TextAreaModel,
    TextInspector,
    "3be4a8c2-6b2e-49dc-896e-b0f357be5f66")

} // namespace SEGMent
