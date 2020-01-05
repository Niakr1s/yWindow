#ifndef TEXTMODEL_H
#define TEXTMODEL_H

#include <QObject>
#include <QPoint>

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

  QStringList getText();

 signals:
  void textChanged();
  void gotTranslation(dict::TranslationChunk, QPoint pos);

 public slots:
  void translate(std::pair<int, int> line_and_col, QPoint pos);
  void addText(QString text);

 protected:
  int current_pos_ = -1;

  virtual QStringList doGetText() = 0;
  virtual dict::TranslationChunk doTranslate(
      std::pair<int, int> line_and_col) = 0;
  virtual void doAddText(const QString& text) = 0;
};

class YomiStyleTextModel : public TextModel {
 public:
  YomiStyleTextModel(dict::Translator* translator, QObject* parent = nullptr);

 protected:
  QStringList doGetText() override;
  dict::TranslationChunk doTranslate(std::pair<int, int> line_and_col) override;
  void doAddText(const QString& text) override;

 private:
  const int max_size_ = 10;
  QStringList text_;
  std::unique_ptr<dict::Translator> translator_;
};

#endif  // TEXTMODEL_H
