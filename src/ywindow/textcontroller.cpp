#include "textcontroller.h"

#include "textmodel.h"

TextController::TextController(QObject *parent) : QObject(parent) {}

void TextController::addText(const std::string &str) {
  addText(QString::fromStdString(str));
}

void TextController::addText(const std::wstring &str) {
  addText(QString::fromStdWString(str));
}

void TextController::addText(const QString &str) {
  QString normalized = str.trimmed();
  normalized.replace("\r\n", " ");
  normalized.replace("\n", " ");
  emit newText(normalized);
}

void TextController::setModel(TextModel *model) {
  model_ = model;
  connect(this, &TextController::newText, model_, &TextModel::addText);
  connect(this, &TextController::needTranslate, model_, &TextModel::translate);
  connect(this, &TextController::needReloadDicts, model_,
          &TextModel::reloadDicts);
  connect(this, &TextController::needAddUserDictionary, model_,
          &TextModel::addUserDictionary);
}

void TextController::charHovered(std::pair<int, int> model_pos, QPoint point,
                                 bool with_shift) {
  emit needTranslate(model_pos, with_shift);
  emit needMoveTranslationView(point);
}
