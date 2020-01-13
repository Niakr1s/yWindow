#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>

#define SETTINGS Settings::getInstance()

class YWindow;
class QTextBrowser;
class SettingsWindow;

class Settings {
 public:
  static Settings* getInstance();

  void sync();

  void loadYWindow(YWindow* w);
  void saveYWindow(YWindow* w);

  void loadTextView(QTextBrowser* w);
  void saveTextView(QTextBrowser* w);

  void loadTranslationView(QTextBrowser* w);
  void saveTranslationView(QTextBrowser* w);

 private:
  Settings();
  const QString TITLE = "yWindow";
  const QString default_style_sheet = "background: black; color: lightGray;";

  const QString QSETTINGS_GEOMETRY = "geometry";
  const QString QSETTINGS_TEXT_FONT = "text_font";
  const QString QSETTINGS_TRANSLATION_FONT = "translation_font";

  QSettings settings_{"Textractor.ini", QSettings::IniFormat};

  void loadTextTranslationViewCommon(QTextBrowser* w);
  void loadFont(QTextBrowser* w, const QString& entry, int sz);
};

#endif  // SETTINGS_H
