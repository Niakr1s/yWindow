#ifndef DISPLAY_H
#define DISPLAY_H

#include <QTextEdit>

class Display : public QTextEdit {
 public:
  Display(QWidget *parent = nullptr);

  void insertText(QString text);

 private:
  void insertEndl(int count);
};

#endif  // DISPLAY_H
