#ifndef YWINDOW_H
#define YWINDOW_H

#include <QLabel>
#include <QObject>
#include <QSettings>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QWidget>

#include "display.h"

class Ywindow : public QWidget {
  Q_OBJECT
 public:
  Ywindow();
  ~Ywindow() override;

 public slots:
  void insertText(QString text);

 private:
  QSettings settings_{"Textractor.ini", QSettings::IniFormat, this};

  QVBoxLayout* vbox_;
  Display* display_;

  void initWindow();
  void initVbox();

  const QString TITLE = "yWindow";

  void saveSettings();

  void insertEndl(int count);
};

#endif  // YWINDOW_H
