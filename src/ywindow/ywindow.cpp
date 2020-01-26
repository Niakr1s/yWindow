#include "ywindow.h"

#include <Windows.h>

#include <QDebug>
#include <QFile>
#include <QMessageBox>
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
  status_ = new Status(this);

  initTextMVC();

  vbox_ = new QVBoxLayout(this);
  vbox_->setMargin(0);
  vbox_->addWidget(text_view_.get());

  status_->setModel(text_model_.get());
  connect(status_, &Status::needChangeOpacity, this, &YWindow::setOpacity);
  vbox_->addWidget(status_);
}

YWindow::~YWindow() {
  SETTINGS->saveYWindow(this);
  SETTINGS->saveTextView(text_view_.get());
  SETTINGS->saveTranslationView(translation_view_.get());
  SETTINGS->saveTranslatorsSettingsView(translators_settings_view_.get());
  SETTINGS->sync();
}

void YWindow::newText(QString text) { return text_controller_->addText(text); }

void YWindow::setOpacity(int opacity) {
  setWindowOpacity(static_cast<qreal>(opacity) / 100);
}

void YWindow::showAbout() {
  QMessageBox::about(
      this, tr("About"),
      tr(R"***(<div>Welcome to yWindow, created by <a href="https://github.com/Niakr1s">Niakr1s</a>!</div>
<div>You can find readme and contact author via <a href="https://github.com/Niakr1s/yWindow">github</a></div>)***"));
}

void YWindow::initTextMVC() {
  //  text_model_ = new YomiStyleTextModel(new
  //  dict::YomiTranslator("yomi_dicts"));
  //  text_model_ =
  //      new FullTranslateTextModel(new dict::YomiTranslator("dicts/yomi"));
  text_model_ = std::make_unique<DefaultModel>(new dict::ChainTranslator(
      {new dict::UserTranslator(Y_USER_DICTS_PATH),
       new dict::YomiTranslator(
           Y_YOMI_DICTS_PATH,
           new dict::DeinflectTranslator(Y_DEINFLECT_DICTS_PATH))}));
  text_model_->setTranslatorsSettings(
      std::make_shared<dict::TranslatorsSettings>(Y_TRANSLATORS_SETTINGS_PATH));

  text_controller_ = std::make_unique<TextController>();
  text_controller_->setModel(text_model_.get());

  text_view_ = std::make_unique<DefaultTextView>(this);
  text_view_->setModel(text_model_.get());
  text_view_->setController(text_controller_.get());
  connect(text_view_.get(), &TextView::needShowAbout, this,
          &YWindow::showAbout);
  SETTINGS->loadTextView(text_view_.get());

  translation_view_ = std::make_unique<DefaultTranslationView>();
  translation_view_->setModel(text_model_.get());
  translation_view_->setController(text_controller_.get());
  SETTINGS->loadTranslationView(translation_view_.get());

  translators_settings_view_ = std::make_unique<TranslatorsSettingsView>();
  translators_settings_view_->setModel(text_model_.get());
  translators_settings_view_->setController(text_controller_.get());
  SETTINGS->loadTranslatorsSettingsView(translators_settings_view_.get());
}
