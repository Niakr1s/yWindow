#ifndef TRANSLATORSSETTINGSVIEW_H
#define TRANSLATORSSETTINGSVIEW_H

#include <QAction>
#include <QMenu>
#include <QPushButton>
#include <QTableWidget>
#include <filesystem>

#include "textcontroller.h"
#include "textmodel.h"
#include "translator.h"

namespace fs = std::filesystem;

class PathAction : public QAction {
 public:
  using QAction::QAction;

  QString path() const;
  void setPath(const QString& path);
  void setPath(const fs::path& path);

 private:
  QString path_;
};

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
  void openUserDictionary(const QString& path);
  void openHoveredDictionary();

  void showMenu(QPoint pos);

 private:
  TextModel* model_;
  TextController* controller_;
  QTableWidget* table_;
  QPushButton *new_dict_btn_, *reload_btn_;

  QMenu* menu_;
  PathAction* open_dict_;

  const int COLS = 3;

  QVector<QTableWidgetItem*> makeRow(const std::string& transl_info,
                                     const std::string& dict_info,
                                     bool enabled);

  void initButtons();
  void initTable();
  void initLayout();
  void initMenu();
};

#endif  // TRANSLATORSSETTINGSVIEW_H
