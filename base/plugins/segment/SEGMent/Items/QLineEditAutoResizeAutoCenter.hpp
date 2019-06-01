#ifndef SEGMENTQLINEEDITAUTORESIZEAUTOCENTER_HPP
#define SEGMENTQLINEEDITAUTORESIZEAUTOCENTER_HPP

#include <QGraphicsProxyWidget>
#include <QLineEdit>

#include <wobjectdefs.h>

class QLineEditAutoResizeAutoCenter : public QLineEdit
{
  W_OBJECT(QLineEditAutoResizeAutoCenter)

public:
  explicit QLineEditAutoResizeAutoCenter(QWidget* parent = Q_NULLPTR);
  explicit QLineEditAutoResizeAutoCenter(
      const QString&,
      QWidget* parent = Q_NULLPTR);

  void FitToTextAndCenterToParent();
  W_SLOT(FitToTextAndCenterToParent)
};

#endif // SEGMENTQLINEEDITAUTORESIZEAUTOCENTER_HPP
