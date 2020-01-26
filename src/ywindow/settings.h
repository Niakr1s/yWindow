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

  const QString DEFAULT_STYLE_SHEET = "background: black; color: lightGray;";

  const QString GEOMETRY = "geometry";
  const QString OPACITY = "opacity";
  const QString TEXT_FONT = "text_font";
  const QString TRANSLATION_FONT = "translation_font";

  const QString YWINDOW_GROUP = "yWindow";
  const QString TEXT_VIEW_GROUP = "text_view";
  const QString TRANSLATION_VIEW_GROUP = "translation_view";
  const QString TRANSLATORS_SETTINGS_VIEW_GROUP = "translators_settings_view";

  QSettings settings_;

  void loadTextTranslationViewCommon(QTextBrowser* w);
};

#endif  // SETTINGS_H
