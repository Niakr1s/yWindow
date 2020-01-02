#include "ywindow.h"

#include <Windows.h>

#include <QDebug>
#include <QFile>
#include <QSizeGrip>
#include <QStyle>
#include <QTextBlock>
#include <QVBoxLayout>

Ywindow::Ywindow(QWidget *parent, Qt::WindowFlags f) : QWidget(parent, f) {
  settings_.beginGroup(TITLE);

  initWindow();

  vbox_ = new QVBoxLayout(this);
  vbox_->setMargin(0);

  display_ = new Display(this);
  vbox_->addWidget(display_);

  status_ = new Status(this);
  vbox_->addWidget(status_);
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
  setWindowFlags(Qt::FramelessWindowHint);

  if (settings_.contains("geometry")) {
    setGeometry(settings_.value("geometry").toRect());
  } else {
    resize(600, 100);
  }
  setWindowOpacity(0.8);
  setStyleSheet("background: black; color: lightGray;");
}

void Ywindow::saveSettings() { settings_.setValue("geometry", geometry()); }
