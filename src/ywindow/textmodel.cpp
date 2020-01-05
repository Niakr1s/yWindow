#include "textmodel.h"

#include <QDebug>

#include "translator.h"

TextModel::TextModel(QObject *parent) : QObject(parent) {}

TextModel::~TextModel() {}

QStringList TextModel::getText() { return doGetText(); }

void TextModel::translate(std::pair<int, int> line_and_col, QPoint pos) {
  auto res = doTranslate(line_and_col);
  emit gotTranslation(res, pos);
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
    std::pair<int, int> line_and_col) {
  QString to_translate = text_[line_and_col.first].mid(line_and_col.second);
  auto res = translator_->translate(to_translate.toStdString(), false);
  return res.chunks().front();
}

void YomiStyleTextModel::doAddText(const QString &text) {
  text_.push_back(text);
  while (text_.size() > max_size_) {
    text_.pop_front();
  }
}
