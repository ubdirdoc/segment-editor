#include <SEGMent/Items/ClickWindow.hpp>
#include <score/selection/SelectionStack.hpp>
#include <QGraphicsSceneDragDropEvent>
#include <SEGMent/Commands/CommandFactory.hpp>
#include <SEGMent/Items/GlobalVariables.hpp>
#include <SEGMent/Commands/Properties.hpp>
#include <QGraphicsWidget>
#include <QPainter>
#include <score/command/Dispatchers/CommandDispatcher.hpp>
namespace SEGMent
{

ClickWindow::ClickWindow(const ClickAreaModel& p,
                         const score::DocumentContext& ctx,
                         QGraphicsItem* parent):
  Window(
      expected_pos(p.pos(), parent->boundingRect())
    , expected_size(p.size(), parent->boundingRect())
    , true, true, ctx, parent)
, m_object{p}
{
  setZValue(5);
  setAcceptDrops(true);

  m_sizeGripItem = new SizeGripItem(new ObjectResizer<ClickWindow, false>{*this}, this, true);

  con(p.selection, &Selectable::changed, this, [=] (bool b) {
    m_anchorsSetter.setVisible(b);
  });
  QObject::connect(
      this, &ClickWindow::on_sizeChanged, this,
      &ClickWindow::sizeChanged);

  ::bind(p, ClickAreaModel::p_pos{}, this, [=] (auto pos) {
    setPos(expected_pos(pos, parentItem()->boundingRect()));
  });
  ::bind(p, ClickAreaModel::p_size{}, this, [=] (auto sz) {
    setRect(expected_rect(sz, parentItem()->boundingRect()));
  });
  ::bind(p, ClickAreaModel::p_z{}, this, [=] (auto z) {
    setZValue(z);
  });

  setMinSize(20, 20);
}

void ClickWindow::sizeChanged()
{
  m_anchorsSetter.updateAnchorsPos();
  if(m_sizeGripItem)
    m_sizeGripItem->reset();
}

void ClickWindow::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
  QGraphicsRectItem::mousePressEvent(event);
  ObjectMover::press(*this, event);
}

void ClickWindow::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
  QGraphicsRectItem::mouseMoveEvent(event);
  ObjectMover::move(*this, event);
}

void ClickWindow::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
  QGraphicsRectItem::mouseReleaseEvent(event);
  ObjectMover::release(*this, event);
}

void ClickWindow::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
  painter->setPen(Style::instance().clickAreaPen);
  painter->setBrush(Style::instance().clickAreaBrush);
  painter->drawRoundedRect(rect(), 2, 2);
  painter->setFont(QFont("Helvetica", 40));
}

void ClickWindow::dropEvent(QGraphicsSceneDragDropEvent* e)
{
  ObjectDropper::dropEvent(*this, ClickAreaModel::p_sound{}, e);
}

void ClickWindow::updateRect()
{
  ObjectMover::updateRect(*this, model().pos(), model().size());
}

transition_t ClickWindow::createTransitionFrom(
    anchor_id source_anchor,
    const SceneModel& target,
    anchor_id target_anchor)
{
  return ClickToScene{model(), target, source_anchor, target_anchor};
}




BackClickWindow::BackClickWindow(const BackClickAreaModel& p,
                         const score::DocumentContext& ctx,
                         QGraphicsItem* parent):
  Window(
    expected_pos(p.pos(), parent->boundingRect())
  , expected_size(p.size(), parent->boundingRect())
  , true, false, ctx, parent)
, m_object{p}
{
  setZValue(5);
  setAcceptDrops(true);

  m_sizeGripItem = new SizeGripItem(new ObjectResizer<BackClickWindow, false>{*this}, this, true);

  con(p.selection, &Selectable::changed, this, [=] (bool b) {
    m_selection = b;
    update();
  });
  QObject::connect(
      this, &BackClickWindow::on_sizeChanged, this,
      &BackClickWindow::sizeChanged);

  ::bind(p, BackClickAreaModel::p_pos{}, this, [=] (auto pos) {
    setPos(expected_pos(pos, parentItem()->boundingRect()));
  });
  ::bind(p, BackClickAreaModel::p_size{}, this, [=] (auto sz) {
    setRect(expected_rect(sz, parentItem()->boundingRect()));
  });
  ::bind(p, BackClickAreaModel::p_z{}, this, [=] (auto z) {
    setZValue(z);
  });

  setMinSize(20, 20);
}

void BackClickWindow::sizeChanged()
{
  m_anchorsSetter.updateAnchorsPos();
  if(m_sizeGripItem)
    m_sizeGripItem->reset();
}

void BackClickWindow::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
  QGraphicsRectItem::mousePressEvent(event);
  ObjectMover::press(*this, event);
}

void BackClickWindow::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
  QGraphicsRectItem::mouseMoveEvent(event);
  ObjectMover::move(*this, event);
}

void BackClickWindow::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
  QGraphicsRectItem::mouseReleaseEvent(event);
  ObjectMover::release(*this, event);
}

void BackClickWindow::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
  if(m_selection)
  {
    QPen pen = Style::instance().backClickAreaPen;
    QBrush brush = Style::instance().backClickAreaBrush;
    pen.setColor(pen.color().lighter());
    brush.setColor(brush.color().darker());
    painter->setPen(pen);
    painter->setBrush(brush);
  }
  else
  {
    painter->setPen(Style::instance().backClickAreaPen);
    painter->setBrush(Style::instance().backClickAreaBrush);
  }

  painter->drawRoundedRect(rect(), 2, 2);
  painter->setFont(QFont("Helvetica", 40));
  painter->drawText(rect(), "Back", QTextOption(Qt::AlignCenter));

}

void BackClickWindow::dropEvent(QGraphicsSceneDragDropEvent* e)
{
  ObjectDropper::dropEvent(*this, BackClickAreaModel::p_sound{}, e);
}

void BackClickWindow::updateRect()
{
  ObjectMover::updateRect(*this, model().pos(), model().size());
}

transition_t BackClickWindow::createTransitionFrom(
    anchor_id source_anchor,
    const SceneModel& target,
    anchor_id target_anchor)
{
  return {};
}







TextWindow::TextWindow(const TextAreaModel& p,
                         const score::DocumentContext& ctx,
                         QGraphicsItem* parent):
  Window(
    expected_pos(p.pos(), parent->boundingRect())
  , expected_size(p.size(), parent->boundingRect())
  , true, false, ctx, parent)
, m_object{p}
{
  setZValue(5);
  setAcceptDrops(true);

  m_sizeGripItem = new SizeGripItem(new ObjectResizer<TextWindow, false>{*this}, this, true);

  con(p.selection, &Selectable::changed, this, [=] (bool b) {
    m_selection = b;
    update();
  });
  QObject::connect(
      this, &TextWindow::on_sizeChanged, this,
      &TextWindow::sizeChanged);

  ::bind(p, TextAreaModel::p_pos{}, this, [=] (auto pos) {
    setPos(expected_pos(pos, parentItem()->boundingRect()));
  });
  ::bind(p, TextAreaModel::p_size{}, this, [=] (auto sz) {
    setRect(expected_rect(sz, parentItem()->boundingRect()));
  });
  ::bind(p, TextAreaModel::p_z{}, this, [=] (auto z) {
    setZValue(z);
  });
  ::bind(p, TextAreaModel::p_text{}, this, [=] (const auto&) {
    update();
  });


  setMinSize(20, 20);
}

int TextWindow::type() const { return static_type(); }

const TextAreaModel&TextWindow::model() const { return m_object; }

void TextWindow::sizeChanged()
{
  m_anchorsSetter.updateAnchorsPos();
  if(m_sizeGripItem)
    m_sizeGripItem->reset();
}


void TextWindow::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
  QGraphicsRectItem::mousePressEvent(event);
  ObjectMover::press(*this, event);
}

void TextWindow::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
  QGraphicsRectItem::mouseMoveEvent(event);
  ObjectMover::move(*this, event);
}

void TextWindow::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
  QGraphicsRectItem::mouseReleaseEvent(event);
  ObjectMover::release(*this, event);
}

void TextWindow::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
  if(m_selection)
  {
    QPen pen = Style::instance().textAreaPen;
    QBrush brush = Style::instance().textAreaBrush;
    pen.setColor(pen.color().lighter());
    brush.setColor(brush.color().darker());
    painter->setPen(pen);
    painter->setBrush(brush);
  }
  else
  {
    painter->setPen(Style::instance().textAreaPen);
    painter->setBrush(Style::instance().textAreaBrush);
  }
  painter->drawRoundedRect(rect(), 2, 2);
  painter->setFont(QFont("Helvetica", 40));
  painter->drawText(rect(), m_object.text(), QTextOption(Qt::AlignCenter));
}

void TextWindow::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
  auto obj = new QGraphicsProxyWidget{this};
  auto l = new QLineEdit;
  l->setText(m_object.text());
  l->setGeometry(rect().toRect());
  l->setFont(QFont("Helvetica", 40));
  obj->setWidget(l);
  l->setFocus(Qt::FocusReason::MouseFocusReason);

  connect(l, &QLineEdit::editingFinished,
          this, [=] {
    CommandDispatcher<> c{context.commandStack};
    if(l->text() != m_object.text())
      c.submitCommand(new SetTextAreaText{m_object, l->text()});
    obj->deleteLater();
  });
}

void TextWindow::dropEvent(QGraphicsSceneDragDropEvent* e)
{
}

void TextWindow::updateRect()
{
  ObjectMover::updateRect(*this, model().pos(), model().size());
}

transition_t TextWindow::createTransitionFrom(
    anchor_id source_anchor,
    const SceneModel& target,
    anchor_id target_anchor)
{
  return {};
}
}
