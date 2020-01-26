#include "textview.h"

#include <QAbstractTextDocumentLayout>
#include <QDebug>
#include <QDesktopServices>
#include <QGuiApplication>
#include <QMouseEvent>
#include <QTextBlock>

#include "textcontroller.h"
#include "textmodel.h"

TextView::TextView(QWidget *parent) : QTextBrowser(parent) {}

TextView::~TextView() {}

void TextView::setController(TextController *controller) {
  controller_ = controller;
  connect(this, &TextView::charHovered, controller_,
          &TextController::charHovered);
  connect(this, &TextView::needShowTranslatorsSettingsView, controller_,
          &TextController::needShowTranslatorsSettingsView);
  connect(this, &TextView::charHoveredPos, controller_,
          &TextController::needMoveTranslationView);
  connect(this, &TextView::charLeaved, controller_,
          &TextController::needHideTranslationView);
}

void TextView::setModel(TextModel *model) {
  model_ = model;
  connect(model_, &TextModel::textChanged, this, &TextView::displayText);
  connect(model_, &TextModel::gotTranslationLength, this,
          &TextView::highlightTranslated);
}

void TextView::displayText() { return doDisplayText(); }

#include "textmodel.h"

DefaultTextView::DefaultTextView(QWidget *parent) : TextView(parent) {
  highlighter_ = new HoverSyntaxHighlighter(document());
  setSearchPaths({"yWindow/templates"});

  initMenu();
}

void DefaultTextView::initMenu() {
  menu_ = new QMenu(this);

  auto show_translators_settings_view_ = new QAction(tr("Settings"));
  connect(show_translators_settings_view_, &QAction::triggered, this,
          &TextView::needShowTranslatorsSettingsView);
  menu_->addAction(show_translators_settings_view_);

  auto open_css_file_ = new QAction(tr("Edit css"));
  connect(open_css_file_, &QAction::triggered, this, [] {
    QDesktopServices::openUrl(
        QUrl::fromLocalFile("yWindow/templates/ytranslation.css"));
  });
  menu_->addAction(open_css_file_);

  menu_->addSeparator();

  auto show_about = new QAction(tr("About"));
  connect(show_about, &QAction::triggered, this, &TextView::needShowAbout);
  menu_->addAction(show_about);

  setContextMenuPolicy(Qt::CustomContextMenu);
  connect(this, &DefaultTextView::customContextMenuRequested, this,
          &DefaultTextView::showMenu);
}

void DefaultTextView::doDisplayText() {
  auto list = model_->toHtml();
  current_text_ = model_->toPlainText();

  list.back() = QString("<a name=\"%1\"></a>%2").arg(anchor_).arg(list.back());

  setHtml(list.join("<br>"));
  moveCursor(QTextCursor::End);

  // https://forum.qt.io/topic/109310/qplaintextedit-bottom-margin-empty-space-below-text
  QTextFrameFormat format = document()->rootFrame()->frameFormat();
  format.setBottomMargin(contentsRect().height());
  document()->rootFrame()->setFrameFormat(format);

  scrollToAnchor(anchor_);
}

void DefaultTextView::mouseMoveEvent(QMouseEvent *event) {
  auto curs = cursorForPosition(event->pos());
  auto current_inner_col = curs.position();
  if (current_inner_col == last_hovered_.inner_col) return;

  highlighter_->reset();
  last_hovered_.inner_col = current_inner_col;
  last_hovered_.inner_pos = {curs.blockNumber(), curs.positionInBlock()};

  auto global_pos = cursorRect(curs).topLeft();
  global_pos = mapToGlobal(global_pos);

  auto current_line_and_col = innerColToModelPos(last_hovered_.inner_col);
  if (current_line_and_col != last_hovered_.model_pos) {
    last_hovered_.model_pos = current_line_and_col;
    qDebug() << "mouse hovered: " << current_line_and_col;
    if (current_line_and_col == NULL_POS_AND_COL) {
      emit charLeaved();
      goto theEnd;
    }
    bool with_shift =
        QGuiApplication::queryKeyboardModifiers() & Qt::ShiftModifier ||
        event->buttons() == Qt::MiddleButton;
    emitCharHovered(current_line_and_col, global_pos, with_shift);
  }
theEnd:
  return QTextBrowser::mouseMoveEvent(event);
}

void DefaultTextView::leaveEvent(QEvent *event) {
  emit charLeaved();
  QTextBrowser::leaveEvent(event);
}

void DefaultTextView::highlightTranslated(int length) {
  highlighter_->highlightSubstr(document(), last_hovered_.inner_pos, length);
}

void DefaultTextView::showMenu(QPoint pos) {
  menu_->move(mapToGlobal(pos));
  menu_->show();
}

std::pair<int, int> DefaultTextView::innerColToModelPos(int pos) {
  int line = 0, col = 0, counter = 0;
  for (int i = 0, i_max = current_text_.size(); i != i_max; ++i) {
    for (int j = 0, j_max = current_text_[i].size(); j != j_max; ++j) {
      if (counter == pos) {
        goto theEnd;
      }
      ++counter;
      ++col;
    }
    if (counter == pos) {
      break;
    }
    ++counter;
    ++line;
    col = 0;
  }
theEnd:
  if (line >= current_text_.size() || col >= current_text_[line].size()) {
    return NULL_POS_AND_COL;
  }
  return {line, col};
}

void DefaultTextView::emitCharHovered(std::pair<int, int> model_pos,
                                      QPoint point, bool with_shift) {
  last_hovered_.model_pos = model_pos;
  emit charHovered(last_hovered_.model_pos, with_shift);
  if (model_pos != NULL_POS_AND_COL && point != QPoint())
    emit charHoveredPos(point);
}
