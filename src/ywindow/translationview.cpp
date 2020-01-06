#include "translationview.h"

#include <QApplication>
#include <QDebug>
#include <QDesktopWidget>
#include <QMouseEvent>
#include <QTimer>

#include "textmodel.h"
#include "translationconverter.h"

TranslationView::TranslationView(QWidget* parent) : QTextBrowser(parent) {
  hide();
  setMaximumHeight(400);
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
    const dict::TranslationChunk& translation, QPoint point) {
  active_ = true;
  return doDisplayTranslation(translation, point);
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
    const dict::TranslationChunk& translation, QPoint point) {
  translation_ = translation;
  qDebug() << "TranslationDisplay: got pos " << point << "height " << height();

  auto tr = translation_.translations();
  qDebug() << "got " << tr.size() << " translations";

  clear();
  setHtml(QString::fromStdString(converter_->toHtml(translation)));
  moveCursor(QTextCursor::Start);

  adjustHeight(document()->size().height());
  point.setY(point.y() - height());
  point.setX(point.x() - 50);
  move(point);
  setGeometry(fittedToDisplay(geometry()));

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

QRect DefaultTranslationView::fittedToDisplay(const QRect& rect) {
  auto desktop = QApplication::desktop()->screenGeometry();
  QRect res(rect);
  if (auto diff_right = res.right() - desktop.right(); diff_right > 0) {
    res.moveRight(desktop.right());
  } else if (auto diff_left = desktop.left() - res.left(); diff_left > 0) {
    res.moveLeft(desktop.left());
  }
  if (auto diff_bot = res.bottom() - desktop.bottom(); diff_bot > 0) {
    res.moveBottom(desktop.bottom());
  } else if (auto diff_top = desktop.top() - res.top(); diff_top > 0) {
    res.moveTop(desktop.top());
  }
  return res;
}

void DefaultTranslationView::adjustHeight(int h) {
  auto g = geometry();
  g.setHeight(h);
  setGeometry(g);
}
