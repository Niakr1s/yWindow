#include "translationview.h"

#include <QDebug>
#include <QMouseEvent>

#include "textmodel.h"

TranslationView::TranslationView(QWidget* parent) : QTextEdit(parent) {
  hide();
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

void DefaultTranslationView::leaveEvent(QEvent* event) {
  hide();
  event->ignore();
}

DefaultTranslationView::DefaultTranslationView(QWidget* widget)
    : TranslationView(widget) {}

void DefaultTranslationView::append(const std::string& str) {
  return QTextEdit::append(QString::fromStdString(str));
}

void DefaultTranslationView::doDisplayTranslation(
    const dict::TranslationChunk& translation, QPoint pos) {
  translation_ = translation;
  qDebug() << "TranslationDisplay: got pos " << pos << "height " << height();
  pos.setY(pos.y() - height());
  move(pos);

  auto tr = translation_.translations();
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
  auto sub_tr = translation_.subTranslations();
  for (auto& t : sub_tr) {
    append(t.second->name());
    append(t.second->reading());
    append(t.second->meaning());
    append(t.second->dictionaryInfo());
    append("------------");
  }
  show();
}
