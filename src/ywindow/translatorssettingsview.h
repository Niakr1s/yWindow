#ifndef TRANSLATORSSETTINGSVIEW_H
#define TRANSLATORSSETTINGSVIEW_H

#include <QTableWidget>

#include "textmodel.h"
#include "translator.h"

class TranslatorsSettingsView : public QWidget {
 public:
  TranslatorsSettingsView(QWidget* parent = nullptr);

  void setTextModel(TextModel* model);

 public slots:
  void show();

  void tableItemClicked(QTableWidgetItem* item);

 private:
  TextModel* model_;
  QTableWidget* table_;

  const int COLS = 3;

  QVector<QTableWidgetItem*> makeRow(const std::string& transl_info,
                                     const std::string& dict_info,
                                     bool enabled);
};

#endif  // TRANSLATORSSETTINGSVIEW_H
