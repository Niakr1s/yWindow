#include "ywindow.h"

#include <Windows.h>

#include <QDebug>
#include <QFile>
#include <QSizeGrip>
#include <QStyle>
#include <QTextBlock>
#include <QVBoxLayout>

#include "paths.h"
#include "settings.h"
#include "textcontroller.h"
#include "textmodel.h"
#include "textview.h"
#include "translator.h"

YWindow::YWindow(QWidget *parent, Qt::WindowFlags f) : QWidget(parent, f) {
  SETTINGS->loadYWindow(this);

  vbox_ = new QVBoxLayout(this);
  vbox_->setMargin(0);

  initTextMVC();
  vbox_->addWidget(text_view_);

  status_ = new Status(this);
  vbox_->addWidget(status_);
}

YWindow::~YWindow() {
  SETTINGS->saveYWindow(this);
  SETTINGS->saveTextView(text_view_);
  SETTINGS->saveTranslationView(translation_view_);
  SETTINGS->sync();
  delete text_model_;
  delete text_view_;
  delete text_controller_;
  delete translation_view_;
}

void YWindow::newText(QString text) { return text_controller_->addText(text); }

void YWindow::initTextMVC() {
  //  text_model_ = new YomiStyleTextModel(new
  //  dict::YomiTranslator("yomi_dicts"));
  //  text_model_ =
  //      new FullTranslateTextModel(new dict::YomiTranslator("dicts/yomi"));
  text_model_ = new DefaultModel(new dict::ChainTranslator(
      {new dict::UserTranslator(Y_USER_DICTS_PATH),
       new dict::YomiTranslator(
           Y_YOMI_DICTS_PATH,
           new dict::DeinflectTranslator(Y_DEINFLECT_DICTS_PATH))}));
  text_model_->setTranslatorsSettings(
      std::make_shared<dict::TranslatorsSettings>(Y_TRANSLATORS_SETTINGS_PATH));

  text_controller_ = new TextController();
  text_controller_->setModel(text_model_);

  text_view_ = new DefaultTextView(this);
  text_view_->setModel(text_model_);
  text_view_->setController(text_controller_);
  SETTINGS->loadTextView(text_view_);

  translation_view_ = new DefaultTranslationView();
  translation_view_->setModel(text_model_);
  SETTINGS->loadTranslationView(translation_view_);

  translators_settings_view_ = new TranslatorsSettingsView();
  translators_settings_view_->setModel(text_model_);
  translators_settings_view_->setController(text_controller_);
}
