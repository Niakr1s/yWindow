#include "settings.h"

#include <QDebug>
#include <QSettings>

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

  setPixelSize(w, settings_.value(QSETTINGS_TEXT_FONT_HEIGHT, 40).toInt());
}

void Settings::saveTextView(QTextBrowser* w) {
  settings_.setValue(QSETTINGS_TEXT_FONT_HEIGHT, w->font().pixelSize());
}

void Settings::loadTranslationView(QTextBrowser* w) {
  loadTextTranslationViewCommon(w);

  setPixelSize(w,
               settings_.value(QSETTINGS_TRANSLATION_FONT_HEIGHT, 12).toInt());

  w->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
  w->setMaximumHeight(400);
  w->resize(400, 200);
}

void Settings::saveTranslationView(QTextBrowser* w) {
  settings_.setValue(QSETTINGS_TRANSLATION_FONT_HEIGHT, w->font().pixelSize());
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

void Settings::setPixelSize(QTextBrowser* w, int sz) {
  QFont f = w->font();
  f.setPixelSize(sz);
  w->setFont(f);
}
