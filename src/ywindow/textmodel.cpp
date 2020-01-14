#include "textmodel.h"

#include <QDebug>

#include "texttohtml.h"

TextModel::TextModel(QObject *parent) : QObject(parent) {}

TextModel::~TextModel() { qDebug() << "TextModel destructor"; }

QStringList TextModel::toHtml() { return doToHtml(); }

QStringList TextModel::toPlainText() { return doToPlainText(); }

void TextModel::setTranslatorsSettings(
    std::shared_ptr<dict::TranslatorsSettings> translators_settings) {
  translators_settings_ = translators_settings;
}

void TextModel::translate(std::pair<int, int> pos, QPoint point,
                          bool with_shift) {
  if (pos == last_pos_) return;
  last_pos_ = pos;
  qDebug() << "TextModel::translate got new " << pos;
  if (last_pos_ == std::pair{-1, -1}) {
    emit cancelTranslation();
    return;
  }
  prepareTranslator();
  auto res =
      with_shift ? doTranslateFromPos(last_pos_) : doTranslate(last_pos_);
  if (res->translated()) {
    emit gotTranslation(res, point);
    if (with_shift) {
      emit gotTranslationLength(
          QString::fromStdString(res->originText()).size());
    }
  }
}

void TextModel::addText(QString text) {
  prepareTranslator();
  qDebug() << " TextModel: adding text: " << text;
  current_pos_ = -1;
  doAddText(text);
  emit textChanged();
}

std::shared_ptr<dict::TranslatorsSettings> TextModel::translators_settings()
    const {
  return translators_settings_;
}

DefaultModel::DefaultModel(dict::Translator *translator, QObject *parent)
    : TextModel(parent),
      translator_(std::unique_ptr<dict::Translator>(translator)) {}

bool DefaultModel::isOnlyHovered() const { return false; }

QStringList DefaultModel::doToHtml() {
  QStringList res;
  TextToHtml converter;

  for (auto &transl_res : text_) {
    for (auto &ch : transl_res.chunks()) {
      if (!ch->translated()) {
        converter.addChunkNoColor(ch->originText());
      } else {
        if (ch->user()) {
          converter.addChunkHighlighted(ch->originText());
        } else {
          converter.nextColor();
          converter.addChunk(ch->originText());
        }
      }
    }
    res.push_back(converter.result());
    converter.clear();
  }

  return res;
}

QStringList DefaultModel::doToPlainText() {
  QStringList res;
  for (auto &text : text_) {
    res.push_back(QString::fromStdString(text.orig_text()));
  }
  return res;
}

dict::TranslationChunkPtr DefaultModel::doTranslate(std::pair<int, int> pos) {
  return text_[pos.first].chunk(pos.second);
}

dict::TranslationChunkPtr DefaultModel::doTranslateFromPos(
    std::pair<int, int> pos) {
  QString to_translate =
      QString::fromStdString(text_[pos.first].orig_text()).mid(pos.second);
  dict::TranslationResult translated =
      translator_->translate(to_translate.toStdString());
  return translated.chunks().front();
}

void DefaultModel::doAddText(const QString &text) {
  text_.push_back(translator_->translate(text.toStdString()));
  while (text_.size() > max_size_) {
    text_.pop_front();
  }
}

void DefaultModel::prepareTranslator() {
  if (!translators_settings_applied_) {
    translator_->setTranslatorsSettings(translators_settings_);
    translators_settings_applied_ = true;
  }
}

std::shared_ptr<dict::TranslatorsSettings> DefaultModel::translators_settings()
    const {
  return translators_settings_;
}
