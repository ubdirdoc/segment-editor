#ifndef SEGMENTCUSTOMWINDOW_HPP
#define SEGMENTCUSTOMWINDOW_HPP

#include "AnchorSetter.hpp"

#include <score/command/Dispatchers/CommandDispatcher.hpp>
#include <score/command/Dispatchers/OngoingCommandDispatcher.hpp>

#include <ossia/detail/ptr_set.hpp>

#include <QGraphicsPixmapItem>
#include <QGraphicsRectItem>
#include <QGraphicsSceneMouseEvent>
#include <QMenu>
#include <QMimeData>
#include <QObject>
#include <QPixmap>

#include <SEGMent/Commands/Deletion.hpp>
#include <SEGMent/Commands/Properties.hpp>
#include <SEGMent/Items/GlobalVariables.hpp>
#include <SEGMent/Items/QLineEditAutoResizeAutoCenter.hpp>
#include <SEGMent/Items/RectItem.hpp>
#include <SEGMent/Items/SizeGripItem.hpp>
#include <SEGMent/Model/ImageModel.hpp>
#include <SEGMent/Model/Transition.hpp>

namespace SEGMent
{
class SceneModel;

//! Base class for most objects in the visual canvas
class Window : public RectItem
{
  W_OBJECT(Window)

public:
  void setImageOpacity(qreal opacity);
  Anchor* anchor(anchor_id) const;

  virtual transition_t createTransitionFrom(
      anchor_id source_anchor,
      const SceneModel& target,
      anchor_id target_anchor)
      = 0;

  const std::vector<Anchor*>& anchors() const noexcept
  {
    return m_anchorsSetter.anchors();
  }

protected:
  Window(
      QPointF pos,
      QSizeF sz,
      bool constrainPosToParent,
      bool anchors,
      const score::DocumentContext& ctx,
      ZoomView& view,
      QGraphicsItem* parent = 0);

  SizeGripItem* m_sizeGripItem{};
  AnchorSetter m_anchorsSetter;
};

//! Generic mechanism to resize items, gifs, etc with undo-redo
template <typename T, bool KeepRatio>
class ObjectResizer final : public SizeGripItem::Resizer
{
public:
  using resize_command = typename score::PropertyCommand_T<
      typename std::remove_reference_t<decltype(
          std::declval<T>().model())>::p_size>::template command<void>::type;

  ObjectResizer(T& w) : self{w} {}
  T& self;
  void start(QGraphicsItem* item) override {}

  void operator()(QGraphicsItem* item, QRectF& rect) override
  {
    auto parent = self.parentItem();
    QSizeF rel;
    const auto prect = parent->boundingRect();
    rel.rwidth() = rect.width() / prect.width();
    if constexpr (KeepRatio)
    {
      const auto cur_rect = self.rect();
      const auto cur_ratio = cur_rect.height() / cur_rect.width();
      rel.rheight() = cur_ratio * rel.rwidth();
    }
    else
    {
      rel.rheight() = rect.height() / prect.width();
    }

    auto new_w = rel.width() * prect.width();
    auto new_h = rel.height() * prect.height();
    if(new_w > 10 && new_h > 10)
    {
      self.context.dispatcher.template submitCommand<resize_command>(
          self.model(), rel);
    }

    rect = self.rect();
  }

  void finish(QGraphicsItem* item) override
  {
    self.context.dispatcher.commit();
  }
};

//! Generic mechanism to move items, gifs, etc with undo-redo
struct ObjectMover
{
  template <typename T>
  using move_command = typename score::PropertyCommand_T<
      typename std::remove_reference_t<decltype(
          std::declval<T>().model())>::p_pos>::template command<void>::type;

  static bool isLeft(QGraphicsSceneMouseEvent* event)
  {
    return (event->buttons() & Qt::LeftButton)
           || (event->button() == Qt::LeftButton);
  }
  static bool isRight(QGraphicsSceneMouseEvent* event)
  {
    return (event->buttons() & Qt::RightButton)
           || (event->button() == Qt::RightButton);
  }

  template <typename T>
  static void press(T& self, QGraphicsSceneMouseEvent* event)
  {
    self.context.selectionStack.pushNewSelection({&self.model()});
    event->accept();
  }

  template <typename T>
  static void move(T& self, QGraphicsSceneMouseEvent* event)
  {
    SCORE_ASSERT(self.parentItem());

    const auto prect = self.parentItem()->boundingRect();

    // Constrain the position of the object
    if (isLeft(event))
    {
      if (self.pos().x() <= 0)
      {
        self.setPos(0, self.pos().y());
      }

      if (self.pos().y() <= 0)
      {
        self.setPos(self.pos().x(), 0);
      }

      if (self.pos().x() + self.boundingRect().width() > prect.width())
      {
        self.setPos(
            prect.width() - self.boundingRect().width(), self.pos().y());
      }

      if (self.pos().y() + self.boundingRect().height() > prect.height())
      {
        self.setPos(
            self.pos().x(), prect.height() - self.boundingRect().height());
      }
      QPointF pt{self.pos().x() / prect.width(),
                 self.pos().y() / prect.height()};
      using command_t =
          typename score::PropertyCommand_T<typename std::remove_reference_t<
              decltype(std::declval<T>().model())>::p_pos>::
              template command<void>::type;
      self.context.dispatcher.template submitCommand<command_t>(
          self.model(), pt);
    }

    event->accept();
  }

  template <typename T>
  static void release(T& self, QGraphicsSceneMouseEvent* event)
  {
    if (isLeft(event))
    {
      if (event->scenePos() != event->buttonDownScenePos(Qt::LeftButton))
      {
        const auto prect = self.parentItem()->boundingRect();
        QPointF pt{self.pos().x() / prect.width(),
                   self.pos().y() / prect.height()};

        using command_t =
            typename score::PropertyCommand_T<typename std::remove_reference_t<
                decltype(std::declval<T>().model())>::p_pos>::
                template command<void>::type;
        self.context.dispatcher.template submitCommand<command_t>(
            self.model(), pt);
        self.context.dispatcher.commit();
      }
      else
      {
        self.context.dispatcher.rollback();
      }
    }
    else if (isRight(event))
    {
      auto m = new QMenu;
      auto act = m->addAction(QObject::tr("Delete"));
      QObject::connect(act, &QAction::triggered, &self, [=, &self] {
        CommandDispatcher<> c{self.context.commandStack};
        c.submitCommand(new RemoveObject{
            (const SEGMent::SceneModel&)*self.model().parent(), self.model()});
      });
      m->exec(event->screenPos());
      m->deleteLater();
    }
    event->accept();
  }

  static void updateRect(RectItem& self, QPointF pos, QSizeF size)
  {
    const auto prect = self.parentItem()->boundingRect();
    qreal parentWidth = prect.width();
    qreal parentHeight = prect.height();

    const auto sz = expected_size(size, prect);
    self.QGraphicsRectItem::setRect(0, 0, sz.width(), sz.height());

    self.setPos(parentWidth * pos.x(), parentHeight * pos.y());

    self.on_sizeChanged();
  }
};

struct ObjectDropper
{
  template <typename T, typename Prop>
  static void dropEvent(T& self, Prop, QGraphicsSceneDragDropEvent* event)
  {
    using cmd =
        typename score::PropertyCommand_T<Prop>::template command<void>::type;

    const QMimeData* currentMimeData = event->mimeData();

    if (currentMimeData->hasUrls())
    {
      if (currentMimeData->text() == SEGMENT_SOUND_ID)
      {
        QUrl currentUrl = currentMimeData->urls().first();

        CommandDispatcher<> disp{self.context.commandStack};
        disp.submitCommand(new cmd{self.model(), currentUrl.path()});
      }
    }

    event->accept();
    return;
  }
};

} // namespace SEGMent

#endif // SEGMENTCUSTOMWINDOW_HPP
