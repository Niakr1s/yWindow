#ifndef YWINDOW_H
#define YWINDOW_H

#include <QLabel>
#include <QObject>
#include <QStatusBar>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QWidget>
#include <memory>

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

  void setOpacity(int opacity);

 private:
  Status* status_;

  const QString TITLE = "yWindow";

  QVBoxLayout* vbox_;

  std::unique_ptr<TextModel> text_model_;
  std::unique_ptr<TextController> text_controller_;

  std::unique_ptr<DefaultTextView> text_view_;
  std::unique_ptr<TranslationView> translation_view_;
  std::unique_ptr<TranslatorsSettingsView> translators_settings_view_;

  void initTextMVC();
};

#endif  // YWINDOW_H
