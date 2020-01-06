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
  connect(this, &TextView::charLeaved, controller_,
          &TextController::charLeaved);
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
  auto list = model_->getText();
  current_text_ = list;
  qDebug() << "Display: got text to display: " << list;

  auto back = list.back();
  list.pop_back();

  setPlainText(list.join("\n"));
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
  auto current_col = curs.position();
  if (current_col != last_hovered_col_) {
    highlighter_->reset();
    last_hovered_col_ = current_col;
    emit charLeaved();
  }
  auto pos = cursorRect(curs).topLeft();
  pos = mapToGlobal(pos);
  try {
    auto new_line_and_pos = posToLineAndPos(last_hovered_col_);
    if (new_line_and_pos != last_line_and_pos_) {
      qDebug() << "mouse hovered: " << new_line_and_pos;
      emit charHovered(new_line_and_pos, pos);
      last_line_and_pos_ = new_line_and_pos;
      event->accept();
    }
  } catch (WrongPosException) {
    event->ignore();
  }
}

void DefaultTextView::leaveEvent(QEvent *event) { emit charLeaved(); }

int DefaultTextView::fontHeight() {
  QFontMetrics fm(font());
  return fm.height();
}

void DefaultTextView::highlightTranslated(int length) {
  highlighter_->highlightSubstr(document(), last_hovered_col_, length);
}

int DefaultTextView::rowsAvailable() {
  auto sz =
      document()->documentLayout()->blockBoundingRect(document()->lastBlock());
  int lines = static_cast<int>(sz.height() + 0.1) / fontHeight();
  int max_rows = height() / fontHeight();
  auto res = std::max(0, max_rows - lines);
  return res;
}

std::pair<int, int> DefaultTextView::posToLineAndPos(int pos) {
  int line = 0, col = 0, counter = 0;
  for (int i = 0, i_max = current_text_.size(); i != i_max; ++i) {
    for (int j = 0, j_max = current_text_[i].size(); j != j_max; ++j) {
      if (counter == pos) {
        return {line, col};
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
  if (line >= current_text_.size() || col >= current_text_[line].size()) {
    throw WrongPosException();
  }
  return {line, col};
}
