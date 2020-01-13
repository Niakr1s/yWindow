#include "settings.h"

#include <QSettings>

#include "ywindow.h"

Settings* Settings::getInstance() {
  static Settings* instance;
  if (!instance) {
    instance = new Settings();
  }
  return instance;
}

void Settings::loadYWindow(YWindow* w) {
  w->setWindowTitle(TITLE);
  w->setGeometry(settings_.value("geometry", QRect(0, 0, 600, 100)).toRect());
  w->setWindowOpacity(0.8);
  w->setStyleSheet("background: black; color: lightGray;");
}

void Settings::saveYWindow(YWindow* w) {
  settings_.setValue("geometry", w->geometry());
}

Settings::Settings() { settings_.beginGroup(TITLE); }
