#ifndef YWINDOW_H
#define YWINDOW_H

#include <QLabel>
#include <QObject>
#include <QSettings>
#include <QStatusBar>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QWidget>

#include "display.h"
#include "status.h"

class Ywindow : public QWidget {
  Q_OBJECT
 public:
  Ywindow(QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
  ~Ywindow() override;

 public slots:
  void insertText(QString text);

 private:
  QSettings settings_{"Textractor.ini", QSettings::IniFormat, this};
  Status* status_;

  QVBoxLayout* vbox_;
  Display* display_;

  void initWindow();

  const QString TITLE = "yWindow";

  void saveSettings();

  void insertEndl(int count);
};

#endif  // YWINDOW_H
