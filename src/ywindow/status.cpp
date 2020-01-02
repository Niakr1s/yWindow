#include "status.h"

#include <QLabel>
#include <QLayout>

Status::Status(QWidget *parent) : QStatusBar(parent) {
  showMessage("Ready");

  auto info = new QLabel("yWindow by Niakr1s", this);
  addPermanentWidget(info);

  setStyleSheet("color: lightGray; border: none black; background: black");
  info->setStyleSheet(
      "color: lightGray; border: none black; background: black");
}
