#pragma once
#include <QDialog>

namespace score
{
class AboutDialog final : public QDialog
{
public:
  AboutDialog(QWidget* parent = 0);

  void mousePressEvent(QMouseEvent* event) override;
  void mouseMoveEvent(QMouseEvent* event) override;

  void paintEvent(QPaintEvent* event) override;

private:
  QSize m_windowSize;
};

} // namespace score
