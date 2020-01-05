#ifndef YOMISTYLETEXTMODEL_H
#define YOMISTYLETEXTMODEL_H

#include <QStringList>

#include "textmodel.h"

class YomiStyleTextModel : public TextModel {
 public:
  YomiStyleTextModel(QObject* parent = nullptr);

 protected:
  QStringList doGetText() override;
  dict::CardPtrList doTranslate(int pos) override;
  void doAddText(const QString& text) override;

 private:
  const int max_size_ = 10;
  QStringList text_;
};

#endif  // YOMISTYLETEXTMODEL_H
