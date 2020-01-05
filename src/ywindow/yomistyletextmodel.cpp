#include "yomistyletextmodel.h"

YomiStyleTextModel::YomiStyleTextModel(QObject *parent) : TextModel(parent) {}

QStringList YomiStyleTextModel::doGetText() { return text_; }

dict::CardPtrList YomiStyleTextModel::doTranslate(int pos) {
  return dict::CardPtrList{};
}

void YomiStyleTextModel::doAddText(const QString &text) {
  text_.push_back(text);
  while (text_.size() > max_size_) {
    text_.pop_front();
  }
}
