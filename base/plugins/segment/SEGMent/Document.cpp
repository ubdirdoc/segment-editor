#include <SEGMent/Document.hpp>
#include <SEGMent/Model/Layer/ProcessView.hpp>
#include <SEGMent/Model/Layer/ProcessPresenter.hpp>
#include <QOpenGLWidget>
#include <QWheelEvent>
#include <core/document/Document.hpp>
#include <core/document/DocumentModel.hpp>
#include <score/tools/IdentifierGeneration.hpp>
#include <QScrollBar>
#include <QMessageBox>
namespace SEGMent
{
DocumentModel::DocumentModel(const score::DocumentContext &ctx, QObject *parent)
  : score::DocumentDelegateModel{Id<score::DocumentDelegateModel>(
                                   score::id_generator::getFirstId()),
                                 "SEGMentDocument", parent}
  , m_context{ctx}
  , m_base{new SEGMent::ProcessModel{Id<SEGMent::ProcessModel>{0}, this}}
{
}

SEGMent::ProcessModel &DocumentModel::process() const
{
  return *m_base;
}

DocumentModel::~DocumentModel()
{
}

void DocumentModel::serialize(const VisitorVariant &vis) const
{
  serialize_dyn(vis, *this);
}


DocumentView::DocumentView(const score::DocumentContext &ctx, QObject *parent)
  : score::DocumentDelegateView{parent}
  , m_view{ctx}
  , m_segmentView{
      new SEGMent::View{
      safe_cast<DocumentModel&>(ctx.document.model().modelDelegate()).process(), ctx, nullptr}
    }
{
  constexpr qreal r = 5000;
  m_segmentView->setPos(-r / 2, -r / 2);
  m_scene.addItem(m_segmentView);
  m_view.setScene(&m_scene);
  m_view.setMinimumSize(400, 400);
  m_view.centerOn(0, 0);
  m_view.setSceneRect({-20000, -20000, 40000, 40000});
  m_view.setCursor(QCursor());
  //m_view.setCacheMode(QGraphicsView::CacheNone);
  //m_view.setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
}

DocumentView::~DocumentView()
{
}

SEGMent::View &DocumentView::view() const
{
  return *m_segmentView;
}

QWidget *DocumentView::getWidget()
{
  return &m_view;
}





ZoomView::ZoomView(const score::DocumentContext& ctx)
  : context{ctx}
{
  setDragMode(QGraphicsView::DragMode::ScrollHandDrag);
}

void ZoomView::enterEvent(QEvent* event)
{
  QGraphicsView::enterEvent(event);
  viewport()->unsetCursor();
}

void ZoomView::mousePressEvent(QMouseEvent* e)
{
  QGraphicsView::mousePressEvent(e);
  auto p = this->mapToScene(e->pos());
  if(QGraphicsItem* item = this->scene()->itemAt(p, transform()); !item || item->type() == View::static_type())
  {
    viewport()->setCursor(Qt::ClosedHandCursor);
     context.selectionStack.deselect();
  }
}

void ZoomView::mouseMoveEvent(QMouseEvent* e)
{
  QGraphicsView::mouseMoveEvent(e);

  if(e->buttons() & Qt::LeftButton)
  {
    auto top_left = mapToScene(QPoint{0,0});
    auto bottom_right = mapToScene(QPoint{width(),height()});
    scene()->update(QRectF{top_left, bottom_right});
  }
}

void ZoomView::mouseReleaseEvent(QMouseEvent* event)
{
  QGraphicsView::mouseReleaseEvent(event);
  viewport()->unsetCursor();
  update();
}

void ZoomView::wheelEvent(QWheelEvent *event)
{
  if (event->modifiers() & Qt::ControlModifier) {
    zoom(event->angleDelta().y());
  } else {
    QGraphicsView::wheelEvent(event);
  }
}

void ZoomView::drawBackground(QPainter *painter, const QRectF &s)
{
  QBrush b = SEGMent::Style::instance().backgroundPen;

  b.setTransform(QTransform(painter->worldTransform().inverted()).scale(2,2));
  painter->setBackground(SEGMent::Style::instance().backgroundBrush);
  painter->setBackgroundMode(Qt::OpaqueMode);
  painter->setBrush(b);
  painter->fillRect(s, b);
}





DocumentPresenter::DocumentPresenter(
    const score::DocumentContext &ctx
    , score::DocumentPresenter *parent_presenter
    , const DocumentModel &delegate_model
    , DocumentView &delegate_view)
  : DocumentDelegatePresenter{parent_presenter, delegate_model,
                              delegate_view}
  , m_pres{new SEGMent::Presenter{delegate_model.process(),
           &delegate_view.view(), ctx, this}}
{
}

DocumentPresenter::~DocumentPresenter()
{
}

void DocumentPresenter::setNewSelection(const Selection &s)
{
  for(auto& obj : m_curSel)
  {
    if(obj)
    {
      auto c = obj->findChild<Selectable*>();
      if(c)
      {
        c->set_impl(false);
      }
    }
  }

  for(auto& obj : s)
  {
    if(obj)
    {
      auto c = obj->findChild<Selectable*>();
      if(c)
      {
        c->set_impl(true);
      }
    }
  }
  m_curSel = s;
}

score::DocumentDelegateView *DocumentFactory::makeView(
    const score::DocumentContext &ctx,
    QObject *parent)
{
  return new DocumentView{ctx, parent};
}

score::DocumentDelegatePresenter *DocumentFactory::makePresenter(
    const score::DocumentContext &ctx,
    score::DocumentPresenter *parent_presenter,
    const score::DocumentDelegateModel &model,
    score::DocumentDelegateView &view)
{
  return new DocumentPresenter{ctx, parent_presenter,
        static_cast<const DocumentModel&>(model),
        static_cast<DocumentView&>(view)};
}

void DocumentFactory::make(
    const score::DocumentContext &ctx,
    score::DocumentDelegateModel *&ptr,
    score::DocumentModel *parent)
{
  std::allocator<DocumentModel> alloc;
  auto res = alloc.allocate(1);
  ptr = res;
  alloc.construct(res, ctx, parent);
}

void DocumentFactory::load(
    const VisitorVariant &vis,
    const score::DocumentContext &ctx,
    score::DocumentDelegateModel *&ptr,
    score::DocumentModel *parent)
{
  std::allocator<DocumentModel> alloc;
  auto res = alloc.allocate(1);
  ptr = res;
  score::deserialize_dyn(vis, [&](auto&& deserializer) {
    alloc.construct(res, deserializer, ctx, parent);
    return ptr;
  });
}


}

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
  doc.m_base = new SEGMent::ProcessModel{JSONObject::Deserializer{obj["Process"].toObject()}, &doc};
}
