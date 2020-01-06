#ifndef TRANSLATIONVIEW_H
#define TRANSLATIONVIEW_H

#include <QTextBrowser>

#include "translationresult.h"

class TextModel;

class TranslationView : public QTextBrowser {
  Q_OBJECT

 public:
  TranslationView(QWidget* parent = nullptr);
  virtual ~TranslationView();

  void setModel(TextModel* model);

 public slots:
  void move(QPoint pos);
  void displayTranslation(const dict::TranslationChunk& translation,
                          QPoint pos);
  void cancelTranslation();

 protected:
  TextModel* model_;
  bool active_ = false;

  virtual void doDisplayTranslation(const dict::TranslationChunk& translation,
                                    QPoint pos) = 0;
  virtual void doCancelTranslation() = 0;
};

class DefaultTranslationView : public TranslationView {
  Q_OBJECT

 public:
  DefaultTranslationView(QWidget* widget = nullptr);

  void append(const std::string& str);

 public slots:
  void tryHideOnTimer();

 protected:
  void doDisplayTranslation(const dict::TranslationChunk& translation,
                            QPoint pos) override;
  void doCancelTranslation() override;

  void leaveEvent(QEvent* event) override;

 private:
  dict::TranslationChunk translation_;
};

#endif  // TRANSLATIONVIEW_H
