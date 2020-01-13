#include "settingswindow.h"

DictionarySettings::DictionarySettings(QWidget *parent) : QWidget(parent) {}

SettingsWindow::SettingsWindow(QWidget *parent) : QTabWidget(parent) {
  addTab(new DictionarySettings(this), "Dictionaries");
}
