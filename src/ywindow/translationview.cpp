#include "translationview.h"

#include <QDebug>
#include <QMouseEvent>
#include <QTimer>

#include "textmodel.h"
#include "translationtohtmlconverter.h"

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
  active_ = true;
  return doDisplayTranslation(translation, pos);
}

void TranslationView::cancelTranslation() {
  if (active_) {
    active_ = false;
    return doCancelTranslation();
  }
}

void DefaultTranslationView::leaveEvent(QEvent* event) {
  doCancelTranslation();
  event->ignore();
}

DefaultTranslationView::DefaultTranslationView(QWidget* widget)
    : TranslationView(widget) {
  resize(400, 200);
  converter_ = std::make_unique<YTranslationToHtmlConverter>();
}

void DefaultTranslationView::append(const std::string& str) {
  return QTextEdit::append(QString::fromStdString(str));
}

void DefaultTranslationView::tryHideOnTimer() {
  if (!underMouse() && !active_) {
    qDebug() << "hiding on timer";
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

  setHtml(QString::fromStdString(converter_->convert(translation)));

  moveCursor(QTextCursor::Start);
  show();
}

void DefaultTranslationView::doCancelTranslation() {
  qDebug() << "Cancel translation timer started";
  QTimer* auto_hide_timer = new QTimer(this);
  auto_hide_timer->setSingleShot(true);
  connect(auto_hide_timer, &QTimer::timeout, this,
          &DefaultTranslationView::tryHideOnTimer);
  auto_hide_timer->start(2000);
}
