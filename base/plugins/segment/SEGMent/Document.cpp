#include <score/tools/IdentifierGeneration.hpp>

#include <core/document/Document.hpp>
#include <core/document/DocumentModel.hpp>

#include <QJsonDocument>
#include <QMessageBox>
#include <QOpenGLWidget>
#include <QSaveFile>
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
  // This code creates all the default folders when making a new document.
  // By default, it is done in a hidden directory - then it is copied to where the user saves it.
  auto locs = QStandardPaths::standardLocations(QStandardPaths::AppDataLocation);
  QDir locs_dir{locs.front()};
  auto doc_folder = QString{"%1-%2"}.arg(QDateTime::currentMSecsSinceEpoch()).arg(ctx.document.metadata().fileName());
  locs_dir.mkpath(doc_folder);
  locs_dir.cd(doc_folder);
  locs_dir.mkdir(SCENE_IMAGES_DIRECTORY);
  locs_dir.mkdir(OBJECT_IMAGES_DIRECTORY);
  locs_dir.mkdir(SOUNDS_DIRECTORY);
  locs_dir.mkdir(TEMPLATES_DIRECTORY);

  auto savename = locs_dir.absolutePath() + QDir::separator() + "temp.segment";
  ctx.document.metadata().setFileName(savename);
  QTimer::singleShot(0, [=] {
    QSaveFile f{savename};
    f.open(QIODevice::WriteOnly);
    f.write(QJsonDocument{m_context.document.saveAsJson()}.toJson());
    f.commit();
  });
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

void DocumentModel::savedDocumentAs(const QString& origPath, const QString& newPath)
{
  auto origDir = QFileInfo{origPath}.absoluteDir();
  auto newDir = QFileInfo{newPath}.absoluteDir();

  copyRecursively(origDir.absolutePath() + QDir::separator() + SCENE_IMAGES_DIRECTORY,
                  newDir.absolutePath()  + QDir::separator() + SCENE_IMAGES_DIRECTORY);

  copyRecursively(origDir.absolutePath() + QDir::separator() + OBJECT_IMAGES_DIRECTORY,
                  newDir.absolutePath()  + QDir::separator() + OBJECT_IMAGES_DIRECTORY);

  copyRecursively(origDir.absolutePath() + QDir::separator() + SOUNDS_DIRECTORY,
                  newDir.absolutePath()  + QDir::separator() + SOUNDS_DIRECTORY);

  copyRecursively(origDir.absolutePath() + QDir::separator() + TEMPLATES_DIRECTORY,
                  newDir.absolutePath()  + QDir::separator() + TEMPLATES_DIRECTORY);
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

QDebug operator<<(QDebug d, const Selection& s)
{
  d << "{";
  for(auto e : s) d << e;
  d << "}";
  return d;
}
void DocumentPresenter::setNewSelection(const Selection& s)
{
  for (auto& obj : m_curSel)
  {
    if (obj && !s.contains(obj))
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
