#ifndef HOVERSYNTAXHIGHLIGHTER_H
#define HOVERSYNTAXHIGHLIGHTER_H

#include <QSyntaxHighlighter>

class HoverSyntaxHighlighter : public QSyntaxHighlighter {
 public:
  HoverSyntaxHighlighter(QObject* parent);
  HoverSyntaxHighlighter(QTextDocument* parent);

  void highlightSubstr(QTextDocument* doc, std::pair<int, int> inner_pos,
                       int length);
  void reset();

 protected:
  void highlightBlock(const QString&) override;

 private:
  std::pair<int, int> last_inner_line_and_col_ = {-1, -1};
  int length_ = -1;
};

#endif  // HOVERSYNTAXHIGHLIGHTER_H
