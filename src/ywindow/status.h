#ifndef STATUS_H
#define STATUS_H

#include <QStatusBar>

#include "textmodel.h"

class Status : public QStatusBar {
  Q_OBJECT

 public:
  Status(QWidget* parent = nullptr);

  void setModel(TextModel* model);

 signals:
  void needChangeOpacity(int);

 public slots:
  void showProcessing();
  void showReady();
};

#endif  // STATUS_H
