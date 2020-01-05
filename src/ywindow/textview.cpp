#include "textview.h"

#include "textcontroller.h"
#include "textmodel.h"

TextView::TextView(QWidget *parent) : QTextEdit(parent) {}

TextView::~TextView() {}

void TextView::setController(TextController *controller) {
  controller_ = controller;
  connect(this, &TextView::charHovered, controller_,
          &TextController::posChanged);
}

void TextView::setModel(TextModel *model) {
  model_ = model;
  connect(model_, &TextModel::textChanged, this, &TextView::displayText);
  connect(model_, &TextModel::gotTranslation, this,
          &TextView::displayTranslation);
}

void TextView::displayText() { return doDisplayText(); }

void TextView::displayTranslation(dict::TranslationChunk translation) {
  return doDisplayTranslation(translation);
}
