#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QTabWidget>
#include <QWidget>

class DictionarySettings : public QWidget {
 public:
  DictionarySettings(QWidget* parent = nullptr);

 private:
};

class SettingsWindow : public QTabWidget {
 public:
  SettingsWindow(QWidget* parent = nullptr);

 private:
};

#endif  // SETTINGSWINDOW_H
