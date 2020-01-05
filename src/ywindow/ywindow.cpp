#include "ywindow.h"

#include <Windows.h>

#include <QDebug>
#include <QFile>
#include <QSizeGrip>
#include <QStyle>
#include <QTextBlock>
#include <QVBoxLayout>

#include "textcontroller.h"
#include "textmodel.h"
#include "textview.h"
#include "translator.h"

Ywindow::Ywindow(QWidget *parent, Qt::WindowFlags f) : QWidget(parent, f) {
  settings_.beginGroup(TITLE);

  initWindow();

  vbox_ = new QVBoxLayout(this);
  vbox_->setMargin(0);

  initTextMVC();
  vbox_->addWidget(text_view_);

  status_ = new Status(this);
  vbox_->addWidget(status_);
}

Ywindow::~Ywindow() { saveSettings(); }

void Ywindow::newText(QString text) { return text_controller_->addText(text); }

void Ywindow::initWindow() {
  setWindowTitle(TITLE);
  //  setWindowFlags(Qt::FramelessWindowHint);

  if (settings_.contains("geometry")) {
    setGeometry(settings_.value("geometry").toRect());
  } else {
    resize(600, 100);
  }
  setWindowOpacity(0.8);
  setStyleSheet("background: black; color: lightGray;");
}

void Ywindow::saveSettings() { settings_.setValue("geometry", geometry()); }

void Ywindow::initTextMVC() {
  text_model_ = new YomiStyleTextModel(new dict::YomiTranslator("yomi_dicts"));

  text_controller_ = new TextController();
  text_controller_->setModel(text_model_);

  text_view_ = new DefaultTextView(this);
  text_view_->setModel(text_model_);
  text_view_->setController(text_controller_);

  translation_view_ = new TranslationDisplay();
  translation_view_->setModel(text_model_);
}

void Ywindow::resizeEvent(QResizeEvent *event) {
  fitToTextView();
  event->accept();
}

void Ywindow::fitToTextView() {
  int font_h = text_view_->fontHeight();
  auto sz = size();
  auto diff_h = sz.height() - text_view_->height();
  sz.setHeight((text_view_->height() / font_h) * font_h + diff_h);
  resize(sz);
}
