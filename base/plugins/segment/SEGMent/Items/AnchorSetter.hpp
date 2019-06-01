#ifndef SEGMENTANCHORSETTER_HPP
#define SEGMENTANCHORSETTER_HPP

#include "Anchor.hpp"

#include <QGraphicsItem>
#include <vector>

namespace SEGMent
{

//! Positions anchors around objects
class AnchorSetter
{
private:
  std::vector<Anchor*> m_anchors;

  QGraphicsItem& m_parent;
  ZoomView& m_view;
  int m_nbAnchorsPerSide;
  qreal m_anchorSize;

public:
  AnchorSetter(
      int nbOfAnchorsPerSide, qreal anchorsSize, ZoomView& view, QGraphicsItem& parent);
  void updateAnchorsPos();

  void setVisible(bool b);
  Anchor* anchor(anchor_id) const noexcept;
  const std::vector<Anchor*>& anchors() const noexcept { return m_anchors; }
};
}

#endif // SEGMENTANCHORSETTER_HPP
