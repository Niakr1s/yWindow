#ifndef TRANSLATORSSETTINGSVIEW_H
#define TRANSLATORSSETTINGSVIEW_H

#include <QPushButton>
#include <QTableWidget>
#include <filesystem>

#include "textcontroller.h"
#include "textmodel.h"
#include "translator.h"

namespace fs = std::filesystem;

class TranslatorsSettingsView : public QWidget {
  Q_OBJECT

 public:
  TranslatorsSettingsView(QWidget* parent = nullptr);

  void setModel(TextModel* model);
  void setController(TextController* controller);

 signals:
  void needAddUserDictionary(const QString& filename);

 public slots:
  void show();
  void redraw();

  void tableItemClicked(QTableWidgetItem* item);
  void addUserDictionary();
  void openUserDictionary(const QString& filename);

 private:
  TextModel* model_;
  TextController* controller_;
  QTableWidget* table_;
  QPushButton *new_dict_btn_, *reload_btn_;

  const int COLS = 3;

  QVector<QTableWidgetItem*> makeRow(const std::string& transl_info,
                                     const std::string& dict_info,
                                     bool enabled);

  void initButtons();
  void initTable();
  void initLayout();
};

#endif  // TRANSLATORSSETTINGSVIEW_H
