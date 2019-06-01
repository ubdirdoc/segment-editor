#include "AnchorSetter.hpp"

#include <QBrush>
#include <QGraphicsScene>
#include <QGraphicsView>

namespace SEGMent
{
AnchorSetter::AnchorSetter(
    int nbOfAnchorsPerSide,
    qreal anchorsSize,
    ZoomView& view,
    QGraphicsItem& parent)
    : m_parent{parent}, m_view{view}
{
  m_nbAnchorsPerSide = nbOfAnchorsPerSide;
  m_anchorSize = anchorsSize;

  for (anchor_id i = 0; i < m_nbAnchorsPerSide * 4; ++i)
  {
    auto anchor = new Anchor{anchorsSize, i, &m_parent, m_view};
    anchor->setVisible(false);
    m_anchors.push_back(anchor);
  }

  updateAnchorsPos();
}

void AnchorSetter::updateAnchorsPos()
{
  QRectF parentBoundingRect = m_parent.boundingRect();

  qreal widthStep = parentBoundingRect.width() / (m_nbAnchorsPerSide + 1.);
  qreal heightStep = parentBoundingRect.height() / (m_nbAnchorsPerSide + 1.);

  for (int i = 0; i < m_nbAnchorsPerSide; ++i)
  {
    // SIDE 1
    m_anchors[i]->setPos((i + 1) * widthStep, 0.);

    // SIDE 2
    m_anchors[i + 1 * m_nbAnchorsPerSide]->setPos(0., (i + 1) * heightStep);

    // SIDE 3
    m_anchors[i + 2 * m_nbAnchorsPerSide]->setPos(
        (i + 1) * widthStep, parentBoundingRect.height());

    // SIDE 4
    m_anchors[i + 3 * m_nbAnchorsPerSide]->setPos(
        parentBoundingRect.width(), (i + 1) * heightStep);
  }
}

void AnchorSetter::setVisible(bool b)
{
  for (auto& anch : m_anchors)
    anch->setVisible(b);
}

Anchor* AnchorSetter::anchor(anchor_id id) const noexcept
{
  if (id >= 0 && id < m_anchors.size())
  {
    return m_anchors[id];
  }
  return nullptr;
}

} // namespace SEGMent
