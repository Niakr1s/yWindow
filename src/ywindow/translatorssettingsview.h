#ifndef TRANSLATORSSETTINGSVIEW_H
#define TRANSLATORSSETTINGSVIEW_H

#include <QPushButton>
#include <QTableWidget>

#include "textcontroller.h"
#include "textmodel.h"
#include "translator.h"

class TranslatorsSettingsView : public QWidget {
 public:
  TranslatorsSettingsView(QWidget* parent = nullptr);

  void setTextModel(TextModel* model);
  void setTextController(TextController* controller);

 public slots:
  void show();
  void redraw();

  void tableItemClicked(QTableWidgetItem* item);

 private:
  TextModel* model_;
  TextController* controller_;
  QTableWidget* table_;
  QPushButton *new_dict_btn_, *reload_btn_;

  const int COLS = 3;

  QVector<QTableWidgetItem*> makeRow(const std::string& transl_info,
                                     const std::string& dict_info,
                                     bool enabled);
};

#endif  // TRANSLATORSSETTINGSVIEW_H
