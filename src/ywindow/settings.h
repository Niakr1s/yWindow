#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>

#define SETTINGS Settings::getInstance()

class YWindow;
class QTextBrowser;
class SettingsWindow;
class TranslatorsSettingsView;

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

  void loadTranslatorsSettingsView(TranslatorsSettingsView* w);
  void saveTranslatorsSettingsView(TranslatorsSettingsView* w);

 private:
  Settings();
  const QString TITLE = "yWindow";
  const QString default_style_sheet = "background: black; color: lightGray;";

  const QString QSETTINGS_GEOMETRY = "geometry";
  const QString QSETTINGS_TEXT_FONT = "text_font";
  const QString QSETTINGS_TRANSLATION_FONT = "translation_font";

  const QString QSETTINGS_TRANSLATORS_SETTINGS_VIEW_GEOMETRY =
      "translators_settings_geometry";

  QSettings settings_;

  void loadTextTranslationViewCommon(QTextBrowser* w);
};

#endif  // SETTINGS_H
