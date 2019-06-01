#include <SEGMent/Items/ObjectWindow.hpp>
#include <score/selection/SelectionStack.hpp>
#include <QGraphicsSceneDragDropEvent>
#include <QMenu>
#include <QMovie>
#include <SEGMent/Commands/CommandFactory.hpp>
#include <SEGMent/Commands/Deletion.hpp>
#include <SEGMent/Commands/Properties.hpp>
#include <score/command/Dispatchers/CommandDispatcher.hpp>
#include <SEGMent/Items/GlobalVariables.hpp>
#include <SEGMent/FilePath.hpp>
#include <QGraphicsTextItem>
#include <QPainter>
namespace SEGMent
{
void WindowWithBackground::sizeChanged()
{
  m_backgroundImgDisplay.setScale(
        boundingRect().width() / (m_backgroundImgRealWidth));

  m_anchorsSetter.updateAnchorsPos();

  if(m_sizeGripItem)
    m_sizeGripItem->reset();
}

void WindowWithBackground::setBackgroundImage(QPixmap img)
{
  m_backgroundImgRealWidth = img.width();
  m_backgroundImgDisplay.setPixmap(img);

  on_sizeChanged();
}

ImageWindow::ImageWindow(const ImageModel& p,
                         const score::DocumentContext& ctx,
                         ZoomView& view,
                         QGraphicsItem* parent):
  ResizableWindow<ImageWindow>{
      expected_pos(p.pos(), parent->boundingRect())
    , expected_size(p.size(), parent->boundingRect())
    ,  true, true, ctx, view, parent}
  , m_object{p}
{
  setMinSize(20, 20);
  setZValue(5);
  setAcceptDrops(true);

  m_sizeGripItem = new SizeGripItem(new ObjectResizer<ImageWindow, true>{*this}, this, true);

  QObject::connect(
      this, &ImageWindow::on_sizeChanged, this,
      &ImageWindow::sizeChanged);

  con(p.selection, &Selectable::changed, this, [=] (bool b) {
    m_anchorsSetter.setVisible(b);
  });
  ::bind(p, ImageModel::p_pos{}, this, [=] (auto pos) {
    setPos(expected_pos(pos, parentItem()->boundingRect()));
  });
  ::bind(p, ImageModel::p_size{}, this, [=] (auto sz) {
    setRect(expected_rect(sz, parentItem()->boundingRect()));
  });
  ::bind(p, ImageModel::p_z{}, this, [=] (auto z) {
    setZValue(z);
  });
  ::bind(p, ImageModel::p_puzzle{}, this, [=] (auto z) {
    setPuzzle(z);
  });
  ::bind(p, ImageModel::p_image{},
         this, [=,&ctx] (const Image& img) {
    setBackgroundImage(QPixmap(toLocalFile(img.path, ctx)));
  });

  setImageOpacity(0.9);
  updateRect();
}

void ImageWindow::setPuzzle(bool b)
{
  if(b)
  {
    m_puzzle = new QGraphicsPixmapItem(QPixmap(":/puzzle.png").scaled(32, 32, Qt::AspectRatioMode::KeepAspectRatio, Qt::FastTransformation), this);
    m_puzzle->setX(boundingRect().width() - m_puzzle->boundingRect().width() / 2.);
    m_puzzle->setY(- m_puzzle->boundingRect().height() / 2.);
  }
  else {
    delete m_puzzle;
    m_puzzle = nullptr;
  }
}


void ImageWindow::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
  QGraphicsRectItem::mousePressEvent(event);
  ObjectMover::press(*this, event);
}

void ImageWindow::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
  QGraphicsRectItem::mouseMoveEvent(event);
  ObjectMover::move(*this, event);
}

void ImageWindow::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
  QGraphicsRectItem::mouseReleaseEvent(event);
  ObjectMover::release(*this, event);
}

void ImageWindow::dropEvent(QGraphicsSceneDragDropEvent* e)
{
  ObjectDropper::dropEvent(*this, ImageModel::p_sound{}, e);
}

transition_t ImageWindow::createTransitionFrom(
    anchor_id source_anchor,
    const SceneModel& target,
    anchor_id target_anchor)
{
  return ObjectToScene{model(), target, source_anchor, target_anchor};
}


GifWindow::GifWindow(const GifModel& p,
                     const score::DocumentContext& ctx,
                     ZoomView& view,
                     QGraphicsItem* parent):
  ResizableWindow<GifWindow>(
    expected_pos(p.pos(), parent->boundingRect())
    , expected_size(p.size(), parent->boundingRect())
    , true, false, ctx, view, parent)
, m_object{p}
{
  setMinSize(20, 20);
  setZValue(5);
  setAcceptDrops(true);

  m_sizeGripItem = new SizeGripItem(new ObjectResizer<GifWindow, true>{*this}, this, true);

  QObject::connect(
      this, &GifWindow::on_sizeChanged, this,
      &GifWindow::sizeChanged);

  con(p.selection, &Selectable::changed, this, [=] (bool b) {
    m_selection = b;
    update();
  });
  ::bind(p, GifModel::p_pos{}, this, [=] (auto pos) {
    setPos(expected_pos(pos, parentItem()->boundingRect()));
  });
  ::bind(p, GifModel::p_size{}, this, [=] (auto sz) {
    setRect(expected_rect(sz, parentItem()->boundingRect()));
  });
  ::bind(p, GifModel::p_z{}, this, [=] (auto z) {
    setZValue(z);
  });
  auto updateimage = [=,&p] (const auto&) {
    p.gif.jumpToFrame(p.defaultFrame());
    setBackgroundImage(p.gif.currentPixmap());
  };

  ::bind(p, GifModel::p_image{}, this, updateimage);
  ::bind(p, GifModel::p_defaultFrame{}, this, updateimage);

  setImageOpacity(0.9);
  updateRect();
}


void GifWindow::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
  QGraphicsRectItem::mousePressEvent(event);
  ObjectMover::press(*this, event);
}

void GifWindow::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
  QGraphicsRectItem::mouseMoveEvent(event);
  ObjectMover::move(*this, event);
}

void GifWindow::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
  QGraphicsRectItem::mouseReleaseEvent(event);
  ObjectMover::release(*this, event);
}

void GifWindow::dropEvent(QGraphicsSceneDragDropEvent* e)
{
  ObjectDropper::dropEvent(*this, GifModel::p_sound{}, e);
}

transition_t GifWindow::createTransitionFrom(
    anchor_id source_anchor,
    const SceneModel& target,
    anchor_id target_anchor)
{
  return GifToScene{model(), target, source_anchor, target_anchor};
}

void GifWindow::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
  if(m_selection)
  {
    QPen pen = Style::instance().sceneBorderPen;
    QBrush brush = QBrush{Qt::transparent};
    painter->setPen(pen);
    painter->setBrush(brush);
    painter->drawRect(boundingRect());
  }
}

}


