#include "translationview.h"

#include "textmodel.h"

TranslationView::TranslationView(QWidget *parent) : QTextEdit(parent) {
  hide();
  setWindowFlags(Qt::FramelessWindowHint);
  setReadOnly(true);
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  viewport()->setCursor(QCursor(Qt::ArrowCursor));
  setFrameStyle(QFrame::NoFrame);
  setMouseTracking(true);

  // TODO merge common settings with Display

  auto pal = palette();
  pal.setColor(QPalette::Text, Qt::lightGray);
  pal.setColor(QPalette::Base, Qt::black);
  pal.setColor(QPalette::Window, Qt::black);
  setPalette(pal);
}

TranslationView::~TranslationView() {}

void TranslationView::setModel(TextModel *model) {
  model_ = model;
  connect(model_, &TextModel::gotTranslation, this,
          &TranslationView::displayTranslation);
}

void TranslationView::move(QPoint pos) { return QWidget::move(pos); }

void TranslationView::displayTranslation(
    const dict::TranslationChunk &translation, QPoint pos) {
  return doDisplayTranslation(translation, pos);
}
