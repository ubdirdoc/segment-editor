#pragma once
#include <score/plugins/documentdelegate/DocumentDelegateFactory.hpp>
#include <score/plugins/documentdelegate/DocumentDelegateModel.hpp>
#include <score/plugins/documentdelegate/DocumentDelegatePresenter.hpp>
#include <score/plugins/documentdelegate/DocumentDelegateView.hpp>
#include <QGraphicsView>
#include <nano_observer.hpp>

namespace SEGMent
{
class ProcessModel;
class View;
class Presenter;

//! The base data model for a SEGMent document.
class DocumentModel final
    : public score::DocumentDelegateModel
    , public Nano::Observer
{
  W_OBJECT(DocumentModel)
  SCORE_SERIALIZE_FRIENDS
  public:
    DocumentModel(const score::DocumentContext& ctx, QObject* parent);

  SEGMent::ProcessModel& process() const;
  template <typename Impl>
  DocumentModel(Impl& vis, const score::DocumentContext& ctx, QObject* parent)
    : score::DocumentDelegateModel{vis, parent}, m_context{ctx}
  {
    vis.writeTo(*this);
  }

  ~DocumentModel() override;

  void serialize(const VisitorVariant& vis) const override;

private:
  const score::DocumentContext& m_context;
  SEGMent::ProcessModel* m_base{};
};

//! A QGraphicsView which allows zooming / dezooming through wheel events
class ZoomView : public QGraphicsView
{
  const score::DocumentContext& context;
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
    if(curscale > 0.01 && curscale < 4)
    {
      scale(factor, factor);
    }
    setTransformationAnchor(anchor);
  }
private:
  void enterEvent(QEvent *event) override;

  void mousePressEvent(QMouseEvent*) override;
  void mouseMoveEvent(QMouseEvent*) override;
  void mouseReleaseEvent(QMouseEvent *event) override;
  void wheelEvent(QWheelEvent *event) override;
  void drawBackground(QPainter* painter, const QRectF& s) override;
};

//! Displays a SEGMent document
class DocumentView final : public score::DocumentDelegateView
{
  W_OBJECT(DocumentView)

  public:
    DocumentView(const score::DocumentContext& ctx, QObject* parent);
  ~DocumentView() override;

  ZoomView& graphicsView() { return m_view; }
  SEGMent::View& view() const;
  QWidget* getWidget() override;

private:
  QGraphicsScene m_scene;
  ZoomView m_view;
  SEGMent::View* m_segmentView{};
};

//! Sets up the view to show the model
class DocumentPresenter final : public score::DocumentDelegatePresenter
{
  W_OBJECT(DocumentPresenter)
  friend class DisplayedElementsPresenter;

public:
  DocumentPresenter(
      const score::DocumentContext& ctx,
      score::DocumentPresenter* parent_presenter,
      const DocumentModel& delegate_model,
      DocumentView& delegate_view);

  ~DocumentPresenter() override;

  void setNewSelection(const Selection& s) override;

private:
  Selection m_curSel;
  SEGMent::Presenter* m_pres{};
};

class DocumentFactory final : public score::DocumentDelegateFactory
{
  SCORE_CONCRETE("800e4012-d34f-4995-800d-3d77953035fd")

  score::DocumentDelegateView*
  makeView(const score::DocumentContext& ctx, QObject* parent) override;

  score::DocumentDelegatePresenter* makePresenter(
      const score::DocumentContext& ctx,
      score::DocumentPresenter* parent_presenter,
      const score::DocumentDelegateModel& model,
      score::DocumentDelegateView& view) override;

  void make(
      const score::DocumentContext& ctx,
      score::DocumentDelegateModel*& ptr,
      score::DocumentModel* parent) override;

  void load(
      const VisitorVariant& vis,
      const score::DocumentContext& ctx,
      score::DocumentDelegateModel*& ptr,
      score::DocumentModel* parent) override;
};

}
