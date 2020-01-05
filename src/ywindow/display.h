#ifndef DISPLAY_H
#define DISPLAY_H

#include <QTextEdit>

#include "textview.h"
#include "translationresult.h"

class Display : public TextView {
 public:
  Display(QWidget *parent = nullptr);

  class WrongPosException : public std::exception {
    using std::exception::exception;
  };

  int fontHeight() override;

 private:
  const QString anchor_ = "last";

 protected:
  void doDisplayText() override;

  void mouseMoveEvent(QMouseEvent *event) override;

 private:
  QStringList current_text_;
  std::pair<int, int> last_line_and_pos_ = {-1, -1};

  int rowsAvailable();
  std::pair<int, int> posToLineAndPos(int pos);
};

#endif  // DISPLAY_H
