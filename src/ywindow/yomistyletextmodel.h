#ifndef YOMISTYLETEXTMODEL_H
#define YOMISTYLETEXTMODEL_H

#include <QStringList>

#include "textmodel.h"
#include "translator.h"

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

#endif  // YOMISTYLETEXTMODEL_H
