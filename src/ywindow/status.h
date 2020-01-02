#ifndef STATUS_H
#define STATUS_H

#include <QStatusBar>

class Status : public QStatusBar {
 public:
  Status(QWidget *parent = nullptr);
};

#endif  // STATUS_H
