#ifndef TEXTVIEW_H
#define TEXTVIEW_H

#include <QString>
#include <QTextBrowser>

#include "card.h"
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

 protected:
  TextController* controller_;
  TextModel* model_;

  virtual void doDisplayText() = 0;
};

class DefaultTextView : public TextView {
 public:
  DefaultTextView(QWidget* parent = nullptr);

  class WrongPosException : public std::exception {
    using std::exception::exception;
  };

  int fontHeight() override;

 private:
  const QString anchor_ = "last";

 protected:
  void doDisplayText() override;

  void mouseMoveEvent(QMouseEvent* event) override;

 private:
  QStringList current_text_;
  std::pair<int, int> last_line_and_pos_ = {-1, -1};

  int rowsAvailable();
  std::pair<int, int> posToLineAndPos(int pos);
};

#endif  // TEXTVIEW_H
