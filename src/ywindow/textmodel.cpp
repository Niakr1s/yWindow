#include "textmodel.h"

#include <QDebug>

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
