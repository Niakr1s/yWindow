#include "status.h"

#include <QLabel>
#include <QLayout>
#include <QSlider>

Status::Status(QWidget *parent) : QStatusBar(parent) {
  showMessage("Ready");

  auto opacity_label = new QLabel(tr("Opacity"));
  addPermanentWidget(opacity_label);

  auto slider = new QSlider(Qt::Horizontal, this);
  slider->setMaximumWidth(100);
  slider->setMinimum(20);
  slider->setMaximum(100);
  slider->setValue(static_cast<int>(parent->windowOpacity() * 100));
  connect(slider, &QSlider::valueChanged, this, &Status::needChangeOpacity);
  addPermanentWidget(slider);

  auto info = new QLabel("yWindow by Niakr1s", this);
  addPermanentWidget(info);

  setStyleSheet("color: lightGray; border: none black; background: black");
  info->setStyleSheet(
      "color: lightGray; border: none black; background: black");
}
