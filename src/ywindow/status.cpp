#include "status.h"

#include <QLabel>
#include <QLayout>
#include <QSlider>

Status::Status(QWidget *parent) : QStatusBar(parent) {
  showReady();

  auto opacity_label = new QLabel(tr("Opacity"));
  addPermanentWidget(opacity_label);

  auto slider = new QSlider(Qt::Horizontal, this);
  slider->setMaximumWidth(100);
  slider->setMinimum(20);
  slider->setMaximum(100);
  slider->setValue(static_cast<int>(parent->windowOpacity() * 100));
  connect(slider, &QSlider::valueChanged, this, &Status::needChangeOpacity);
  addPermanentWidget(slider);

  setStyleSheet("color: lightGray; border: none black; background: black");
}

void Status::setModel(TextModel *model) {
  connect(model, &TextModel::startProcessing, this, &Status::showProcessing);
  connect(model, &TextModel::endProcessing, this, &Status::showReady);
}

void Status::showProcessing() { showMessage(tr("Processing...")); }

void Status::showReady() { showMessage(tr("Ready")); }
