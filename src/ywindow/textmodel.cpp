#include "textmodel.h"

#include <QDebug>

#include "translator.h"

TextModel::TextModel(QObject *parent) : QObject(parent) {}

TextModel::~TextModel() {}

QStringList TextModel::getText() { return doGetText(); }

void TextModel::translate(std::pair<int, int> pos, QPoint point) {
  if (pos == last_pos_) return;
  last_pos_ = pos;
  qDebug() << "TextModel::translate got new " << pos;
  if (last_pos_ == std::pair{-1, -1}) {
    emit cancelTranslation();
    return;
  }
  auto res = doTranslate(last_pos_);
  if (res.translated()) {
    emit gotTranslation(res, point);
    emit gotTranslationLength(QString::fromStdString(res.text()).size());
  }
}

void TextModel::addText(QString text) {
  qDebug() << " TextModel: adding text: " << text;
  current_pos_ = -1;
  doAddText(text);
  emit textChanged();
}

YomiStyleTextModel::YomiStyleTextModel(dict::Translator *translator,
                                       QObject *parent)
    : TextModel(parent),
      translator_(std::unique_ptr<dict::Translator>(translator)) {}

QStringList YomiStyleTextModel::doGetText() { return text_; }

dict::TranslationChunk YomiStyleTextModel::doTranslate(
    std::pair<int, int> pos) {
  QString to_translate = text_[pos.first].mid(pos.second);
  auto res = translator_->translate(to_translate.toStdString(), false);
  qDebug() << "YomiStyleTextModel::doTranslate: transl.size"
           << res.chunks().front().translations().size();
  return res.chunks().front();
}

void YomiStyleTextModel::doAddText(const QString &text) {
  text_.push_back(text);
  while (text_.size() > max_size_) {
    text_.pop_front();
  }
}
