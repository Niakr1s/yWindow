#ifndef YWINDOW_H
#define YWINDOW_H

#include <QLabel>
#include <QObject>
#include <QSettings>
#include <QStatusBar>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QWidget>

#include "status.h"
#include "textview.h"
#include "translationview.h"

class Ywindow : public QWidget {
  Q_OBJECT
 public:
  Ywindow(QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
  ~Ywindow() override;

 public slots:
  void newText(QString text);  // entry point

 private:
  QSettings settings_{"Textractor.ini", QSettings::IniFormat, this};
  Status* status_;

  const QString TITLE = "yWindow";

  QVBoxLayout* vbox_;

  TextModel* text_model_;
  DefaultTextView* text_view_;
  TranslationView* translation_view_;
  TextController* text_controller_;

  void initWindow();
  void saveSettings();
  void initTextMVC();

  void resizeEvent(QResizeEvent* event) override;
  void fitToTextView();
};

#endif  // YWINDOW_H
