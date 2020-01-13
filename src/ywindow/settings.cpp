#include "settings.h"

#include <QDebug>
#include <QFont>
#include <QSettings>

#include "settingswindow.h"
#include "ywindow.h"

Settings::Settings() { settings_.beginGroup(TITLE); }

Settings* Settings::getInstance() {
  static Settings* instance;
  if (!instance) {
    instance = new Settings();
  }
  return instance;
}

void Settings::sync() { settings_.sync(); }

void Settings::loadYWindow(YWindow* w) {
  w->setWindowTitle(TITLE);
  w->setGeometry(
      settings_.value(QSETTINGS_GEOMETRY, QRect(0, 0, 600, 100)).toRect());
  w->setWindowOpacity(0.8);
  w->setStyleSheet(default_style_sheet);
}

void Settings::saveYWindow(YWindow* w) {
  settings_.setValue(QSETTINGS_GEOMETRY, w->geometry());
}

void Settings::loadTextView(QTextBrowser* w) {
  loadTextTranslationViewCommon(w);
}

void Settings::saveTextView(QTextBrowser* w) {
  settings_.setValue(QSETTINGS_TEXT_FONT, w->font());
}

void Settings::loadTranslationView(QTextBrowser* w) {
  loadTextTranslationViewCommon(w);

  w->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
  w->setMaximumHeight(400);
  w->resize(400, 200);
}

void Settings::saveTranslationView(QTextBrowser* w) {
  settings_.setValue(QSETTINGS_TRANSLATION_FONT, w->font());
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
  w->setStyleSheet(default_style_sheet);

  auto pal = w->palette();
  pal.setColor(QPalette::Text, Qt::lightGray);
  pal.setColor(QPalette::Base, Qt::black);
  pal.setColor(QPalette::Window, Qt::black);
  w->setPalette(pal);
}
