#ifndef STATUS_H
#define STATUS_H

#include <QStatusBar>

class Status : public QStatusBar {
  Q_OBJECT

 public:
  Status(QWidget *parent = nullptr);

 signals:
  void needChangeOpacity(int);
};

#endif  // STATUS_H
