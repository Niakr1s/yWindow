#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>

#define SETTINGS Settings::getInstance()

class YWindow;

class Settings {
 public:
  static Settings* getInstance();

  void loadYWindow(YWindow* w);
  void saveYWindow(YWindow* w);

 private:
  Settings();
  const QString TITLE = "yWindow";

  QSettings settings_{"Textractor.ini", QSettings::IniFormat};
};

#endif  // SETTINGS_H
