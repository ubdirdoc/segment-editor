#include "QLineEditAutoResizeAutoCenter.hpp"

#include <wobjectimpl.h>
W_OBJECT_IMPL(QLineEditAutoResizeAutoCenter)

QLineEditAutoResizeAutoCenter::QLineEditAutoResizeAutoCenter(QWidget* parent)
    : QLineEdit(parent)
{
  connect(this, &QLineEdit::textChanged, this, [this] {
    FitToTextAndCenterToParent();
  });

  FitToTextAndCenterToParent();
}

QLineEditAutoResizeAutoCenter::QLineEditAutoResizeAutoCenter(
    const QString& init,
    QWidget* parent)
    : QLineEdit(init, parent)
{
  connect(this, &QLineEdit::textChanged, this, [this] {
    FitToTextAndCenterToParent();
  });
  FitToTextAndCenterToParent();
}

void QLineEditAutoResizeAutoCenter::FitToTextAndCenterToParent()
{
  QGraphicsProxyWidget* currentProxyWidget = this->graphicsProxyWidget();

  qreal widthOffset = 10;

  QString text = this->text();
  QFontMetrics fm = this->fontMetrics();
  int w = fm.boundingRect(text).width();

  if (currentProxyWidget != nullptr)
  {
    auto rect = currentProxyWidget->parentItem()->boundingRect();

    if (w + widthOffset > rect.width())
    {
      this->resize(rect.width(), rect.height());
      this->move(rect.x(), rect.y());
    }
    else
    {
      this->resize(w + widthOffset, rect.height());
      this->move((rect.x() + (rect.width() - w) / 2), rect.y());
    }
  }
}
