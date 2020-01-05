#ifndef DISPLAY_H
#define DISPLAY_H

#include <QTextEdit>

#include "textview.h"

class Display : public TextView, public QTextEdit {
 public:
  Display(QWidget *widg = nullptr, QObject *obj = nullptr);

  class WrongPosException : public std::exception {
    using std::exception::exception;
  };

  int fontHeight() override;

 private:
  const QString anchor_ = "last";

 protected:
  void doDisplayText() override;
  void doDisplayTranslation(const dict::CardPtrList &translation) override;

  void mouseMoveEvent(QMouseEvent *event) override;

 private:
  QStringList current_text_;
  std::pair<int, int> last_line_and_pos_ = {-1, -1};

  int rowsAvailable();
  std::pair<int, int> posToLineAndPos(int pos);
};

#endif  // DISPLAY_H
