#include "translationdisplay.h"

TranslationDisplay::TranslationDisplay(QWidget *widget) : QTextEdit(widget) {}

void TranslationDisplay::append(const std::string &str) {
  return QTextEdit::append(QString::fromStdString(str));
}
