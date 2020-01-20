#ifndef YWINDOW_H
#define YWINDOW_H

#include <QLabel>
#include <QObject>
#include <QStatusBar>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QWidget>

#include "status.h"
#include "textview.h"
#include "translationview.h"
#include "translatorssettingsview.h"

class YWindow : public QWidget {
  Q_OBJECT
 public:
  YWindow(QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
  ~YWindow() override;

 public slots:
  void newText(QString text);  // entry point

 private:
  Status* status_;

  const QString TITLE = "yWindow";

  QVBoxLayout* vbox_;

  TextModel* text_model_;
  DefaultTextView* text_view_;
  TranslationView* translation_view_;
  TextController* text_controller_;
  TranslatorsSettingsView* translators_settings_view_;

  void initTextMVC();
};

#endif  // YWINDOW_H
