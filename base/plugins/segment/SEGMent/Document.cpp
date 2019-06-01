#include <score/tools/IdentifierGeneration.hpp>

#include <core/document/Document.hpp>
#include <core/document/DocumentModel.hpp>

#include <QMessageBox>
#include <QOpenGLWidget>
#include <QScrollBar>
#include <QWheelEvent>

#include <SEGMent/Commands/Creation.hpp>
#include <SEGMent/Document.hpp>
#include <SEGMent/Model/Layer/ProcessPresenter.hpp>
#include <SEGMent/Model/Layer/ProcessView.hpp>
namespace SEGMent
{
DocumentModel::DocumentModel(
    const score::DocumentContext& ctx,
    QObject* parent)
    : score::DocumentDelegateModel{Id<score::DocumentDelegateModel>(
                                       score::id_generator::getFirstId()),
                                   "SEGMentDocument",
                                   parent}
    , m_context{ctx}
    , m_base{new SEGMent::ProcessModel{Id<SEGMent::ProcessModel>{0}, this}}
{
}

SEGMent::ProcessModel& DocumentModel::process() const
{
  return *m_base;
}

DocumentModel::~DocumentModel() {}

void DocumentModel::serialize(const VisitorVariant& vis) const
{
  serialize_dyn(vis, *this);
}

DocumentView::DocumentView(const score::DocumentContext& ctx, QObject* parent)
    : score::DocumentDelegateView{parent}, m_view{ctx}
{
  constexpr qreal w = 50000.;
  m_view.setScene(&m_scene);
  m_view.setMinimumSize(400, 400);
  m_view.centerOn(0, 0);
  m_view.setSceneRect({-w / 2., -w / 2., w, w});
  m_view.setCursor(QCursor());
}

DocumentView::~DocumentView() {}

SEGMent::ZoomView& DocumentView::view() const
{
  return const_cast<ZoomView&>(m_view);
}

QWidget* DocumentView::getWidget()
{
  return &m_view;
}

DocumentPresenter::DocumentPresenter(
    const score::DocumentContext& ctx,
    score::DocumentPresenter* parent_presenter,
    const DocumentModel& delegate_model,
    DocumentView& delegate_view)
    : DocumentDelegatePresenter{parent_presenter,
                                delegate_model,
                                delegate_view}
    , m_pres{new SEGMent::Presenter{delegate_model.process(),
                                    &delegate_view.view(),
                                    ctx,
                                    this}}
{
}

DocumentPresenter::~DocumentPresenter() {}

void DocumentPresenter::setNewSelection(const Selection& s)
{
  for (auto& obj : m_curSel)
  {
    if (obj)
    {
      auto c = obj->findChild<Selectable*>();
      if (c)
      {
        c->set_impl(false);
      }
    }
  }

  for (auto& obj : s)
  {
    if (obj)
    {
      auto c = obj->findChild<Selectable*>();
      if (c)
      {
        c->set_impl(true);
      }
    }
  }
  m_curSel = s;
}

score::DocumentDelegateView*
DocumentFactory::makeView(const score::DocumentContext& ctx, QObject* parent)
{
  return new DocumentView{ctx, parent};
}

score::DocumentDelegatePresenter* DocumentFactory::makePresenter(
    const score::DocumentContext& ctx,
    score::DocumentPresenter* parent_presenter,
    const score::DocumentDelegateModel& model,
    score::DocumentDelegateView& view)
{
  return new DocumentPresenter{ctx,
                               parent_presenter,
                               static_cast<const DocumentModel&>(model),
                               static_cast<DocumentView&>(view)};
}

void DocumentFactory::make(
    const score::DocumentContext& ctx,
    score::DocumentDelegateModel*& ptr,
    score::DocumentModel* parent)
{
  std::allocator<DocumentModel> alloc;
  auto res = alloc.allocate(1);
  ptr = res;
  alloc.construct(res, ctx, parent);
}

void DocumentFactory::load(
    const VisitorVariant& vis,
    const score::DocumentContext& ctx,
    score::DocumentDelegateModel*& ptr,
    score::DocumentModel* parent)
{
  std::allocator<DocumentModel> alloc;
  auto res = alloc.allocate(1);
  ptr = res;
  score::deserialize_dyn(vis, [&](auto&& deserializer) {
    alloc.construct(res, deserializer, ctx, parent);
    return ptr;
  });
}

} // namespace SEGMent

W_OBJECT_IMPL(SEGMent::DocumentModel)
W_OBJECT_IMPL(SEGMent::DocumentPresenter)
W_OBJECT_IMPL(SEGMent::DocumentView)
template <>
void DataStreamReader::read(const SEGMent::DocumentModel& obj)
{
  readFrom(*obj.m_base);
  insertDelimiter();
}

template <>
void DataStreamWriter::write(SEGMent::DocumentModel& doc)
{
  doc.m_base = new SEGMent::ProcessModel{*this, &doc};
  checkDelimiter();
}

template <>
void JSONObjectReader::read(const SEGMent::DocumentModel& doc)
{
  obj["Process"] = toJsonObject(*doc.m_base);
}

template <>
void JSONObjectWriter::write(SEGMent::DocumentModel& doc)
{
  doc.m_base = new SEGMent::ProcessModel{
      JSONObject::Deserializer{obj["Process"].toObject()}, &doc};
}
