#include "ywindow.h"

#include <QDebug>
#include <QFile>
#include <QSizeGrip>
#include <QStyle>
#include <QTextBlock>
#include <QVBoxLayout>

Ywindow::Ywindow() {
  settings_.beginGroup(TITLE);

  initWindow();
  initVbox();
  display_ = new Display(this);
  vbox_->addWidget(display_);
}

Ywindow::~Ywindow() { saveSettings(); }

void Ywindow::insertText(QString text) { return display_->insertText(text); }

void Ywindow::insertEndl(int count) {
  while (count--) {
    display_->insertPlainText("\n");
  }
}

void Ywindow::initWindow() {
  setWindowTitle(TITLE);

  if (settings_.contains("geometry")) {
    setGeometry(settings_.value("geometry").toRect());
  } else {
    resize(600, 100);
  }
  setWindowOpacity(0.8);
  auto pal = palette();
  pal.setColor(QPalette::Background, Qt::black);
  setPalette(pal);
}

void Ywindow::initVbox() {
  vbox_ = new QVBoxLayout(this);
  vbox_->setMargin(0);
}

void Ywindow::saveSettings() { settings_.setValue("geometry", geometry()); }
