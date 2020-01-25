#ifndef TRANSLATIONVIEW_H
#define TRANSLATIONVIEW_H

#include <QTextBrowser>

#include "translationresult.h"

class TextModel;
class TextController;
class TranslationConverter;

class TranslationView : public QTextBrowser {
  Q_OBJECT

 public:
  TranslationView(QWidget* parent = nullptr);
  virtual ~TranslationView();

  void setModel(TextModel* model);
  void setController(TextController* controller);

 public slots:
  virtual void move(QPoint pos) = 0;
  void displayTranslation(dict::TranslationChunkPtr translation);
  void cancelTranslation();

 protected:
  TextModel* model_;
  TextController* controller_;
  bool active_ = false;

  virtual void doDisplayTranslation(dict::TranslationChunkPtr translation) = 0;
  virtual void doCancelTranslation() = 0;
};

class DefaultTranslationView : public TranslationView {
  Q_OBJECT

 public:
  DefaultTranslationView(QWidget* widget = nullptr);

  void append(const std::string& str);

 public slots:
  void move(QPoint pos);
  void tryHideOnTimer();

 protected:
  void doDisplayTranslation(dict::TranslationChunkPtr translation) override;
  void doCancelTranslation() override;

  void leaveEvent(QEvent* event) override;

 private:
  dict::TranslationChunkPtr translation_;
  std::unique_ptr<TranslationConverter> converter_;

  QRect fittedToDisplay(const QRect& rect);
  void adjustHeight(int h);
};

#endif  // TRANSLATIONVIEW_H
