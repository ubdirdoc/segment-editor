#pragma once
#include <score/plugins/documentdelegate/DocumentDelegateFactory.hpp>
#include <score/plugins/documentdelegate/DocumentDelegateModel.hpp>
#include <score/plugins/documentdelegate/DocumentDelegatePresenter.hpp>
#include <score/plugins/documentdelegate/DocumentDelegateView.hpp>

#include <QGraphicsView>

#include <SEGMent/Items/Arrow.hpp>
#include <SEGMent/Items/SceneWindow.hpp>
#include <nano_observer.hpp>

namespace SEGMent
{
class ProcessModel;
class ZoomView;
class Presenter;

//! The base data model for a SEGMent document.
class DocumentModel final : public score::DocumentDelegateModel,
                            public Nano::Observer
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
  void savedDocumentAs(const QString& oldpath, const QString& newpath) override;

private:
  const score::DocumentContext& m_context;
  SEGMent::ProcessModel* m_base{};
};

//! Displays a SEGMent document
class DocumentView final : public score::DocumentDelegateView
{
  W_OBJECT(DocumentView)

public:
  DocumentView(const score::DocumentContext& ctx, QObject* parent);
  ~DocumentView() override;

  ZoomView& graphicsView() { return m_view; }
  SEGMent::ZoomView& view() const;
  QWidget* getWidget() override;

private:
  QGraphicsScene m_scene;
  ZoomView m_view;
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

} // namespace SEGMent
