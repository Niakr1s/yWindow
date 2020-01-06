#ifndef TEXTVIEW_H
#define TEXTVIEW_H

#include <QString>
#include <QTextBrowser>

#include "card.h"
#include "hoversyntaxhighlighter.h"
#include "translationresult.h"

class TextController;
class TextModel;

class TextView : public QTextBrowser {
  Q_OBJECT

 public:
  TextView(QWidget* parent = nullptr);
  virtual ~TextView();

  void setController(TextController* controller);
  void setModel(TextModel* model);

  virtual int fontHeight() = 0;

 signals:
  void charHovered(std::pair<int, int> line_and_col, QPoint pos);

 public slots:
  void displayText();
  virtual void highlightTranslated(int length) = 0;

 protected:
  TextController* controller_;
  TextModel* model_;

  virtual void doDisplayText() = 0;
};

class DefaultTextView : public TextView {
  Q_OBJECT

 public:
  DefaultTextView(QWidget* parent = nullptr);

  class WrongPosException : public std::exception {
    using std::exception::exception;
  };

  int fontHeight() override;

 public slots:
  void highlightTranslated(int length) override;

 private:
  const QString anchor_ = "last";

 protected:
  void doDisplayText() override;

  void mouseMoveEvent(QMouseEvent* event) override;
  void leaveEvent(QEvent* event) override;

 private:
  QStringList current_text_;
  std::pair<int, int> last_line_and_col_ = {-1, -1};
  int last_hovered_inner_col_ = -1;
  HoverSyntaxHighlighter* highlighter_;

  int rowsAvailable();
  std::pair<int, int> colToLineAndCol(int pos);

  void emitCharHovered(std::pair<int, int> line_and_col, QPoint pos);
};

#endif  // TEXTVIEW_H
