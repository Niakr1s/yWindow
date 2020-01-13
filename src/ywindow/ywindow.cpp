#include "ywindow.h"

#include <Windows.h>

#include <QDebug>
#include <QFile>
#include <QSizeGrip>
#include <QStyle>
#include <QTextBlock>
#include <QVBoxLayout>

#include "settings.h"
#include "textcontroller.h"
#include "textmodel.h"
#include "textview.h"
#include "translator.h"
#include "ystyler.h"

YWindow::YWindow(QWidget *parent, Qt::WindowFlags f) : QWidget(parent, f) {
  SETTINGS->loadYWindow(this);

  vbox_ = new QVBoxLayout(this);
  vbox_->setMargin(0);

  initTextMVC();
  vbox_->addWidget(text_view_);

  status_ = new Status(this);
  vbox_->addWidget(status_);
}

YWindow::~YWindow() { SETTINGS->saveYWindow(this); }

void YWindow::newText(QString text) { return text_controller_->addText(text); }

void YWindow::initTextMVC() {
  //  text_model_ = new YomiStyleTextModel(new
  //  dict::YomiTranslator("yomi_dicts"));
  //  text_model_ =
  //      new FullTranslateTextModel(new dict::YomiTranslator("dicts/yomi"));
  text_model_ = new DefaultModel(new dict::ChainTranslator(
      {new dict::UserTranslator("dicts/user.txt"),
       new dict::YomiTranslator("dicts/yomi", new dict::DeinflectTranslator(
                                                  "dicts/deinflect.json"))}));

  text_controller_ = new TextController();
  text_controller_->setModel(text_model_);

  text_view_ = new DefaultTextView(this);
  text_view_->setModel(text_model_);
  text_view_->setController(text_controller_);

  translation_view_ = new DefaultTranslationView();
  translation_view_->setModel(text_model_);

  YStyler ystyler;
  ystyler.appendStyleTextView(text_view_, 40);
  ystyler.appendStyleTranslationView(translation_view_, 12);
}

void YWindow::resizeEvent(QResizeEvent *event) {
  fitToTextView();
  event->accept();
}

void YWindow::fitToTextView() {
  int font_h = text_view_->fontHeight();
  auto sz = size();
  auto diff_h = sz.height() - text_view_->height();
  sz.setHeight((text_view_->height() / font_h) * font_h + diff_h);
  resize(sz);
}
