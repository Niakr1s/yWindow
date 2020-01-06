#include "translationview.h"

#include <QDebug>
#include <QMouseEvent>
#include <QTimer>

#include "textmodel.h"

TranslationView::TranslationView(QWidget* parent) : QTextBrowser(parent) {
  hide();
}

TranslationView::~TranslationView() {}

void TranslationView::setModel(TextModel* model) {
  model_ = model;
  connect(model_, &TextModel::gotTranslation, this,
          &TranslationView::displayTranslation);
  connect(model_, &TextModel::cancelTranslation, this,
          &TranslationView::cancelTranslation);
}

void TranslationView::move(QPoint pos) { return QWidget::move(pos); }

void TranslationView::displayTranslation(
    const dict::TranslationChunk& translation, QPoint pos) {
  if (translation.translated()) {
    active_ = true;
    return doDisplayTranslation(translation, pos);
  } else {
    active_ = false;
  }
}

void TranslationView::cancelTranslation() {
  active_ = false;
  return doCancelTranslation();
}

void DefaultTranslationView::leaveEvent(QEvent* event) {
  hide();
  event->ignore();
}

DefaultTranslationView::DefaultTranslationView(QWidget* widget)
    : TranslationView(widget) {
  resize(400, 200);
}

void DefaultTranslationView::append(const std::string& str) {
  return QTextEdit::append(QString::fromStdString(str));
}

void DefaultTranslationView::tryHideOnTimer() {
  if (!underMouse() && !active_) {
    hide();
  }
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
  moveCursor(QTextCursor::Start);
  show();
}

void DefaultTranslationView::doCancelTranslation() {
  QTimer* auto_hide_timer = new QTimer(this);
  connect(auto_hide_timer, &QTimer::timeout, this,
          &DefaultTranslationView::tryHideOnTimer);
  auto_hide_timer->start(500);
}
