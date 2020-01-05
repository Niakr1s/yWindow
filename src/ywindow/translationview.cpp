#include "translationview.h"

#include <QDebug>

#include "textmodel.h"

TranslationView::TranslationView(QWidget* parent) : QTextEdit(parent) {
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

void TranslationView::setModel(TextModel* model) {
  model_ = model;
  connect(model_, &TextModel::gotTranslation, this,
          &TranslationView::displayTranslation);
}

void TranslationView::move(QPoint pos) { return QWidget::move(pos); }

void TranslationView::displayTranslation(
    const dict::TranslationChunk& translation, QPoint pos) {
  return doDisplayTranslation(translation, pos);
}

TranslationDisplay::TranslationDisplay(QWidget* widget)
    : TranslationView(widget) {}

void TranslationDisplay::append(const std::string& str) {
  return QTextEdit::append(QString::fromStdString(str));
}

void TranslationDisplay::doDisplayTranslation(
    const dict::TranslationChunk& translation, QPoint pos) {
  qDebug() << "TranslationDisplay: got pos " << pos << "height " << height();
  pos.setY(pos.y() - height());
  move(pos);

  auto tr = translation.translations();
  qDebug() << "got " << tr.size() << " translations";
  clear();
  for (auto& t : tr) {
    append(t.second->name());
    append(t.second->reading());
    append(t.second->meaning());
    append(t.second->dictionaryInfo());
    append("------------");
  }

  // TODO refactor
  auto sub_tr = translation.subTranslations();
  for (auto& t : sub_tr) {
    append(t.second->name());
    append(t.second->reading());
    append(t.second->meaning());
    append(t.second->dictionaryInfo());
    append("------------");
  }
  show();
}
