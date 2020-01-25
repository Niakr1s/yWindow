#include "translationview.h"

#include <QApplication>
#include <QDebug>
#include <QDesktopWidget>
#include <QMouseEvent>
#include <QTimer>

#include "textcontroller.h"
#include "textmodel.h"
#include "translationconverter.h"

TranslationView::TranslationView(QWidget* parent) : QTextBrowser(parent) {
  hide();
}

TranslationView::~TranslationView() {}

void TranslationView::setModel(TextModel* model) {
  model_ = model;
  connect(model_, &TextModel::gotTranslation, this,
          &TranslationView::displayTranslation);
  connect(model_, &TextModel::cancelTranslation, this,
          &TranslationView::hideTranslation);
}

void TranslationView::setController(TextController* controller) {
  controller_ = controller;
  connect(controller_, &TextController::needMoveTranslationView, this,
          &TranslationView::move);
  connect(controller_, &TextController::needHideTranslationView, this,
          &TranslationView::hideTranslation);
}

void DefaultTranslationView::move(QPoint pos) {
  pos.setY(pos.y() - height());
  pos.setX(pos.x() - 50);
  QWidget::move(pos);
  setGeometry(fittedToDisplay(geometry()));
}

void TranslationView::displayTranslation(
    dict::TranslationChunkPtr translation) {
  active_ = true;
  return doDisplayTranslation(translation);
}

void TranslationView::hideTranslation() {
  if (active_) {
    active_ = false;
    return doHideTranslation();
  }
}

void DefaultTranslationView::leaveEvent(QEvent* event) {
  doHideTranslation();
  event->ignore();
}

DefaultTranslationView::DefaultTranslationView(QWidget* widget)
    : TranslationView(widget) {
  converter_ = std::make_unique<YTranslationConverter>();
  setSearchPaths({"yWindow/templates"});
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
    dict::TranslationChunkPtr translation) {
  translation_ = translation;

  auto tr = translation_->translations();
  qDebug() << "got " << tr.size() << " translations";

  clear();
  setHtml(QString::fromStdString(converter_->toHtml(*translation)));
  moveCursor(QTextCursor::Start);

  adjustHeight(document()->size().height());

  show();
}

void DefaultTranslationView::doHideTranslation() {
  qDebug() << "Cancel translation timer started";
  QTimer* auto_hide_timer = new QTimer(this);
  auto_hide_timer->setSingleShot(true);
  connect(auto_hide_timer, &QTimer::timeout, this,
          &DefaultTranslationView::tryHideOnTimer);
  auto_hide_timer->start(2000);
}

QRect DefaultTranslationView::fittedToDisplay(const QRect& rect) {
  const int offset = 5;
  auto desktop = QApplication::desktop()->screenGeometry();
  QRect res(rect);
  if (auto diff_right = res.right() - desktop.right(); diff_right > 0) {
    res.moveRight(desktop.right() - offset);
  } else if (auto diff_left = desktop.left() - res.left(); diff_left > 0) {
    res.moveLeft(desktop.left() + offset);
  }
  if (auto diff_bot = res.bottom() - desktop.bottom(); diff_bot > 0) {
    res.moveBottom(desktop.bottom() - offset);
  } else if (auto diff_top = desktop.top() - res.top(); diff_top > 0) {
    res.moveTop(desktop.top() + offset);
  }
  return res;
}

void DefaultTranslationView::adjustHeight(int h) {
  auto g = geometry();
  g.setHeight(h);
  setGeometry(g);
}
