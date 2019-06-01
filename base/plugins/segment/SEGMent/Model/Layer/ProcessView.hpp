#pragma once
#include <QGraphicsView>
#include <SEGMent/Items/GlobalVariables.hpp>
#include <SEGMent/Items/SceneWindow.hpp>
#include <SEGMent/Model/SimpleObject.hpp>
#include <SEGMent/Model/ProcessModel.hpp>

namespace SEGMent
{
class Arrow;

//! This class iterates on all scenes to find a child object which matches with a model.
//! TODO if this becomes a performance bottleneck one day, replace this by a hash_map<model*, view*>
template<typename Model_T, typename View_T>
struct window_visitor
{
    Model_T& request;
    Window* operator()(SceneWindow* parent)
    {
      for(auto cld : parent->childWindows())
      {
        if(cld->type() == View_T::static_type())
        {
          auto obj = static_cast<View_T*>(cld);
          if(&obj->model() == &request)
            return obj;
        }
      }
      return nullptr;
    }
};

class RectItem;
//! A QGraphicsView which allows zooming / dezooming through wheel events
//! SEGMent Canvas. Receives scenes drops and handles arrow creation
class ZoomView : public QGraphicsView
{
  W_OBJECT(ZoomView)
public:
  ZoomView(const score::DocumentContext& ctx);

  void zoom(int amount)
  {
    const ViewportAnchor anchor = transformationAnchor();
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    int angle = amount;
    qreal factor;

    if (angle > 0) {
      factor = 1.1 - std::clamp(0.1 / std::abs(angle), 0., 0.1);
    } else {
      factor = 0.9 + std::clamp(0.1 / std::abs(angle), 0., 0.1);
    }

    double curscale = transform().m11() * factor;
    if(curscale > 0.001 && curscale < 4)
    {
      scale(factor, factor);
    }
    setTransformationAnchor(anchor);
  }

  void setStartAnchorForNewArrow(Anchor& startAnchor);
  void setEndAnchorForNewArrow(Anchor& endAnchor);
  void finishArrowDrop();

  Window* findItem(ImageModel* obj);
  template<typename View, typename Model>
  Window* findItem(Model* obj)
  {
    if(!obj)
      return nullptr;

    for(auto v : m_sceneWindows)
    {
      if(auto p = window_visitor<Model, View>{*obj}(v))
        return p;
    }
    return nullptr;
  }


  void addScene(SceneWindow* s);

  void removeScene(const SceneWindow* s);
  void addTransition(Arrow* a);
  void removeTransition(const Arrow* a);

  void dragMove(QPointF pos);

  void doubleClicked() W_SIGNAL(doubleClicked);

private:
  void enterEvent(QEvent *event) override;

  void mousePressEvent(QMouseEvent*) override;
  void mouseMoveEvent(QMouseEvent*) override;
  void mouseReleaseEvent(QMouseEvent *event) override;
  void wheelEvent(QWheelEvent *event) override;
  void drawBackground(QPainter* painter, const QRectF& s) override;

  void dropEvent(QDropEvent* event) override;
  void dragMoveEvent(QDragMoveEvent* event) override;

  const score::DocumentContext& context;
  const ProcessModel& m_process;

  std::vector<SceneWindow*> m_sceneWindows;
  std::vector<SEGMent::Arrow*> m_sceneArrows;
  Anchor* m_startAnchorForNewArrow{};

  QGraphicsLineItem* m_tmpArrow{};
};
}
