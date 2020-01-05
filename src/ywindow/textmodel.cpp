#include "textmodel.h"

#include <QDebug>

TextModel::TextModel(QObject *parent) : QObject(parent) {}

TextModel::~TextModel() {}

QStringList TextModel::getText() { return doGetText(); }

void TextModel::translate(int pos) {
  if (pos == current_pos_) {
    return;
  }
  emit gotTranslation(doTranslate(pos));
}

void TextModel::addText(QString text) {
  qDebug() << " TextModel: adding text: " << text;
  current_pos_ = -1;
  doAddText(text);
  emit textChanged();
}
