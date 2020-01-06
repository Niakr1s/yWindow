#include "hoversyntaxhighlighter.h"

#include <QDebug>

HoverSyntaxHighlighter::HoverSyntaxHighlighter(QObject *parent)
    : QSyntaxHighlighter(parent) {}

HoverSyntaxHighlighter::HoverSyntaxHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent) {}

void HoverSyntaxHighlighter::highlightSubstr(QTextDocument *doc, int pos,
                                             int length) {
  qDebug() << "HoverSyntaxHighlighter::highlightSubstr: got pos " << pos
           << ", length " << length;
  pos_ = pos;
  length_ = length;
  setDocument(doc);
}

void HoverSyntaxHighlighter::highlightBlock(const QString &) {
  if (pos_ == -1 || length_ == -1) return;

  QTextCharFormat format;
  format.setBackground(Qt::blue);
  setFormat(pos_, length_, format);
}

void HoverSyntaxHighlighter::reset() {
  pos_ = -1;
  length_ = -1;
  setDocument(nullptr);
}
