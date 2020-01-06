#include "hoversyntaxhighlighter.h"

#include <QDebug>

HoverSyntaxHighlighter::HoverSyntaxHighlighter(QObject *parent)
    : QSyntaxHighlighter(parent) {}

HoverSyntaxHighlighter::HoverSyntaxHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent) {}

void HoverSyntaxHighlighter::highlightSubstr(QTextDocument *doc,
                                             std::pair<int, int> inner_pos,
                                             int length) {
  last_inner_line_and_col_ = inner_pos;
  length_ = length;
  setDocument(doc);
}

void HoverSyntaxHighlighter::highlightBlock(const QString &) {
  if (last_inner_line_and_col_ == std::pair{-1, -1} || length_ == -1) return;
  if (currentBlock().blockNumber() != last_inner_line_and_col_.first) return;
  QTextCharFormat format;
  format.setBackground(Qt::blue);
  setFormat(last_inner_line_and_col_.second, length_, format);
}

void HoverSyntaxHighlighter::reset() {
  last_inner_line_and_col_ = {-1, -1};
  length_ = -1;
  setDocument(nullptr);
}
