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

 protected:
  TextModel* model_;

  virtual void doDisplayTranslation(const dict::TranslationChunk& translation,
                                    QPoint pos) = 0;
};

class DefaultTranslationView : public TranslationView {
 public:
  DefaultTranslationView(QWidget* widget = nullptr);

  void append(const std::string& str);

 protected:
  void doDisplayTranslation(const dict::TranslationChunk& translation,
                            QPoint pos) override;
  void leaveEvent(QEvent* event) override;

 private:
  dict::TranslationChunk translation_;
};

#endif  // TRANSLATIONVIEW_H
