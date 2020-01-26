#include "settings.h"

#include <QDebug>
#include <QFont>
#include <QSettings>

#include "paths.h"
#include "settingswindow.h"
#include "translatorssettingsview.h"
#include "ywindow.h"

Settings::Settings()
    : settings_(QSettings{QString::fromStdString(Y_SETTINGS_PATH.string()),
                          QSettings::IniFormat}) {}

Settings* Settings::getInstance() {
  static Settings* instance;
  if (!instance) {
    instance = new Settings();
  }
  return instance;
}

void Settings::sync() { settings_.sync(); }

void Settings::loadYWindow(YWindow* w) {
  settings_.beginGroup(YWINDOW_GROUP);
  w->setWindowTitle(TITLE);
  w->setGeometry(settings_.value(GEOMETRY, QRect(0, 0, 600, 100)).toRect());
  w->setWindowOpacity(settings_.value(OPACITY, 0.9).toReal());
  w->setStyleSheet(DEFAULT_STYLE_SHEET);
  settings_.endGroup();
}

void Settings::saveYWindow(YWindow* w) {
  settings_.beginGroup(YWINDOW_GROUP);
  settings_.setValue(GEOMETRY, w->geometry());
  settings_.setValue(OPACITY, w->windowOpacity());
  settings_.endGroup();
}

void Settings::loadTextView(QTextBrowser* w) {
  settings_.beginGroup(TEXT_VIEW_GROUP);
  loadTextTranslationViewCommon(w);
  settings_.endGroup();
}

void Settings::saveTextView(QTextBrowser* w) {
  settings_.beginGroup(TEXT_VIEW_GROUP);
  settings_.setValue(TEXT_FONT, w->font());
  settings_.endGroup();
}

void Settings::loadTranslationView(QTextBrowser* w) {
  settings_.beginGroup(TRANSLATION_VIEW_GROUP);
  loadTextTranslationViewCommon(w);

  w->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
  w->setMaximumHeight(400);
  w->resize(400, 200);
  settings_.endGroup();
}

void Settings::saveTranslationView(QTextBrowser* w) {
  settings_.beginGroup(TRANSLATION_VIEW_GROUP);
  settings_.setValue(TRANSLATION_FONT, w->font());
  settings_.endGroup();
}

void Settings::loadTranslatorsSettingsView(TranslatorsSettingsView* w) {
  settings_.beginGroup(TRANSLATORS_SETTINGS_VIEW_GROUP);
  w->setGeometry(settings_.value(GEOMETRY, w->geometry()).toRect());
  settings_.endGroup();
}

void Settings::saveTranslatorsSettingsView(TranslatorsSettingsView* w) {
  settings_.beginGroup(TRANSLATORS_SETTINGS_VIEW_GROUP);
  settings_.setValue(GEOMETRY, w->geometry());
  settings_.endGroup();
}

void Settings::loadTextTranslationViewCommon(QTextBrowser* w) {
  w->setAlignment(Qt::AlignTop | Qt::AlignLeft);
  w->setWordWrapMode(QTextOption::WordWrap);
  w->setReadOnly(true);
  w->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  w->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  w->viewport()->setCursor(QCursor(Qt::ArrowCursor));
  w->setFrameStyle(QFrame::NoFrame);
  w->setMouseTracking(true);

  w->setWindowOpacity(0.8);
  w->setStyleSheet(DEFAULT_STYLE_SHEET);

  auto pal = w->palette();
  pal.setColor(QPalette::Text, Qt::lightGray);
  pal.setColor(QPalette::Base, Qt::black);
  pal.setColor(QPalette::Window, Qt::black);
  w->setPalette(pal);
}
