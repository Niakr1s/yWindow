#include "textmodel.h"

#include <QDebug>

#include "texttohtml.h"
#include "translator.h"

TextModel::TextModel(QObject *parent) : QObject(parent) {}

TextModel::~TextModel() {}

QStringList TextModel::toHtml() { return doToHtml(); }

QStringList TextModel::toPlainText() { return doToPlainText(); }

void TextModel::translate(std::pair<int, int> pos, QPoint point) {
  if (pos == last_pos_) return;
  last_pos_ = pos;
  qDebug() << "TextModel::translate got new " << pos;
  if (last_pos_ == std::pair{-1, -1}) {
    emit cancelTranslation();
    return;
  }
  auto res = doTranslate(last_pos_);
  if (res->translated()) {
    emit gotTranslation(res, point);
    emit gotTranslationLength(QString::fromStdString(res->originText()).size());
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

QStringList YomiStyleTextModel::doToHtml() { return text_; }

QStringList YomiStyleTextModel::doToPlainText() { return text_; }

dict::TranslationChunkPtr YomiStyleTextModel::doTranslate(
    std::pair<int, int> pos) {
  QString to_translate = text_[pos.first].mid(pos.second);
  auto res = translator_->translate(to_translate.toStdString());
  qDebug() << "YomiStyleTextModel::doTranslate: transl.size"
           << res.chunks().front()->translations().size();
  return res.chunks().front();
}

void YomiStyleTextModel::doAddText(const QString &text) {
  text_.push_back(text);
  while (text_.size() > max_size_) {
    text_.pop_front();
  }
}

FullTranslateTextModel::FullTranslateTextModel(dict::Translator *translator,
                                               QObject *parent)
    : TextModel(parent),
      translator_(std::unique_ptr<dict::Translator>(translator)) {}

QStringList FullTranslateTextModel::doToHtml() {
  QStringList res;
  TextToHtml converter({"green", "blue"});

  for (auto &transl_res : text_) {
    for (auto &ch : transl_res.chunks()) {
      if (!ch->translated()) {
        converter.addChunkNoColor(ch->originText());
      } else {
        converter.nextColor();
        converter.addChunk(ch->originText());
      }
    }
    res.push_back(converter.result());
    converter.clear();
  }

  return res;
}

QStringList FullTranslateTextModel::doToPlainText() {
  QStringList res;
  for (auto &text : text_) {
    res.push_back(QString::fromStdString(text.orig_text()));
  }
  return res;
}

dict::TranslationChunkPtr FullTranslateTextModel::doTranslate(
    std::pair<int, int> pos) {
  return text_[pos.first].chunk(pos.second);
}

void FullTranslateTextModel::doAddText(const QString &text) {
  text_.push_back(translator_->translate(text.toStdString()));
  while (text_.size() > max_size_) {
    text_.pop_front();
  }
}
