#ifndef TEXTMODEL_H
#define TEXTMODEL_H

#include <QObject>
#include <QPoint>
#include <deque>

#include "card.h"
#include "translationresult.h"

namespace dict {
class Translator;
}

class TextModel : public QObject {
  Q_OBJECT

 public:
  TextModel(QObject* parent = nullptr);
  virtual ~TextModel();

  QStringList toHtml();
  QStringList toPlainText();

 signals:
  void textChanged();
  void gotTranslation(dict::TranslationChunkPtr, QPoint point);
  void gotTranslationLength(int);
  void cancelTranslation();

 public slots:
  void translate(std::pair<int, int> pos, QPoint point);
  void addText(QString text);

 protected:
  std::pair<int, int> last_pos_ = {-1, -1};
  int current_pos_ = -1;

  virtual QStringList doToHtml() = 0;
  virtual QStringList doToPlainText() = 0;
  virtual dict::TranslationChunkPtr doTranslate(std::pair<int, int> pos) = 0;
  virtual void doAddText(const QString& text) = 0;
};

class YomiStyleTextModel : public TextModel {
 public:
  YomiStyleTextModel(dict::Translator* translator, QObject* parent = nullptr);

 protected:
  QStringList doToHtml() override;
  QStringList doToPlainText() override;
  dict::TranslationChunkPtr doTranslate(std::pair<int, int> pos) override;
  void doAddText(const QString& text) override;

 private:
  const int max_size_ = 10;
  QStringList text_;
  std::unique_ptr<dict::Translator> translator_;
};

class FullTranslateTextModel : public TextModel {
 public:
  FullTranslateTextModel(dict::Translator* translator,
                         QObject* parent = nullptr);

 protected:
  QStringList doToHtml() override;
  QStringList doToPlainText() override;
  dict::TranslationChunkPtr doTranslate(std::pair<int, int> pos) override;
  void doAddText(const QString& text) override;

 private:
  const size_t max_size_ = 10;
  std::deque<dict::TranslationResult> text_;
  std::unique_ptr<dict::Translator> translator_;
};

#endif  // TEXTMODEL_H
