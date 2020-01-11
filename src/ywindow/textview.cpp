#include "textview.h"

#include <QAbstractTextDocumentLayout>
#include <QDebug>
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
}

void DefaultTextView::doDisplayText() {
  auto list = model_->toHtml();
  current_text_ = model_->toPlainText();
  qDebug() << "Display: got text to display: " << list;

  auto back = list.back();
  list.pop_back();

  setHtml(list.join("<br>"));
  moveCursor(QTextCursor::End);
  if (!list.empty()) {
    insertPlainText("\n");
  }

  auto html_back = QString("<a name=\"%1\">%2</a>").arg(anchor_).arg(back);
  insertHtml(html_back);
  moveCursor(QTextCursor::End);

  for (int i = 0, i_max = rowsAvailable(); i != i_max; ++i) {
    insertPlainText("\n");
  }

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
    qDebug() << "mouse hovered: " << current_line_and_col;
    emitCharHovered(current_line_and_col, global_pos);
    last_hovered_.model_pos = current_line_and_col;
  }
  return QTextBrowser::mouseMoveEvent(event);
}

void DefaultTextView::leaveEvent(QEvent *event) {
  emitCharHovered({-1, -1}, QPoint());
  QTextBrowser::leaveEvent(event);
}

int DefaultTextView::fontHeight() {
  QFontMetrics fm(font());
  return fm.height();
}

void DefaultTextView::highlightTranslated(int length) {
  highlighter_->highlightSubstr(document(), last_hovered_.inner_pos, length);
}

int DefaultTextView::rowsAvailable() {
  auto sz =
      document()->documentLayout()->blockBoundingRect(document()->lastBlock());
  int lines = static_cast<int>(sz.height() + 0.1) / fontHeight();
  int max_rows = height() / fontHeight();
  auto res = std::max(0, max_rows - lines);
  return res;
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
    return {-1, -1};
  }
  return {line, col};
}

void DefaultTextView::emitCharHovered(std::pair<int, int> model_pos,
                                      QPoint point) {
  last_hovered_.model_pos = model_pos;
  emit charHovered(last_hovered_.model_pos, point);
}
