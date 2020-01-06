#ifndef HOVERSYNTAXHIGHLIGHTER_H
#define HOVERSYNTAXHIGHLIGHTER_H

#include <QSyntaxHighlighter>

class HoverSyntaxHighlighter : public QSyntaxHighlighter {
 public:
  HoverSyntaxHighlighter(QObject* parent);
  HoverSyntaxHighlighter(QTextDocument* parent);

  void highlightSubstr(QTextDocument* doc, int pos, int length);
  void reset();

 protected:
  void highlightBlock(const QString&) override;

 private:
  int pos_ = -1, length_ = -1;
};

#endif  // HOVERSYNTAXHIGHLIGHTER_H
