#include "translatorssettingsview.h"

#include <QButtonGroup>
#include <QDebug>
#include <QDesktopServices>
#include <QErrorMessage>
#include <QHeaderView>
#include <QInputDialog>
#include <QMessageBox>
#include <QScrollBar>
#include <QUrl>
#include <QVBoxLayout>
#include <filesystem>

#include "paths.h"
#include "translator.h"

namespace fs = std::filesystem;

TranslatorsSettingsView::TranslatorsSettingsView(QWidget *parent)
    : QWidget(parent) {
  initButtons();
  initTable();
  initLayout();
  initMenu();
}

void TranslatorsSettingsView::setModel(TextModel *model) {
  model_ = model;
  connect(model_, &TextModel::translatorSettingsChanged, this,
          &TranslatorsSettingsView::redraw);
  connect(model_, &TextModel::userDictionaryCreated, this,
          &TranslatorsSettingsView::openUserDictionary);
}

void TranslatorsSettingsView::setController(TextController *controller) {
  controller_ = controller;
  connect(reload_btn_, &QPushButton::clicked, controller_,
          &TextController::needReloadDicts);
  connect(this, &TranslatorsSettingsView::needAddUserDictionary, controller_,
          &TextController::needAddUserDictionary);
  connect(controller_, &TextController::needShowTranslatorsSettingsView, this,
          &TranslatorsSettingsView::show);
}

void TranslatorsSettingsView::show() {
  QWidget::show();
  activateWindow();
}

void TranslatorsSettingsView::redraw() {
  if (!model_) return;
  auto ts = model_->translators_settings();
  if (!ts) return;
  table_->clear();

  table_->setColumnCount(COLS);
  table_->setColumnWidth(COLS - 1, 5);
  table_->setHorizontalHeaderLabels({"Translator", "Dictionary", ""});
  table_->setRowCount(ts->size());
  table_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  table_->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
  table_->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);

  qDebug() << "TranslatorSettingsView: row count" << ts->size();

  int current_row = 0;
  auto &settings = ts->settings();
  for (auto &[trans_info, dict_settings] : settings) {
    for (auto &dict_info : dict_settings.enabled) {
      QVector<QTableWidgetItem *> items = makeRow(trans_info, dict_info, true);
      for (int col = 0; col != COLS; ++col) {
        table_->setItem(current_row, col, items[col]);
      }
      ++current_row;
    }
    for (auto &dict_info : dict_settings.disabled) {
      auto items = makeRow(trans_info, dict_info, false);
      for (int col = 0; col != COLS; ++col) {
        table_->setItem(current_row, col, items[col]);
      }
      ++current_row;
    }
  }
}

void TranslatorsSettingsView::tableItemClicked(QTableWidgetItem *item) {
  if (item->column() != COLS - 1) return;

  int row = item->row();
  auto transl_info = table_->item(row, 0)->text().toStdString();
  auto dict_info = table_->item(row, 1)->text().toStdString();
  auto enabled = item->checkState();
  model_->translators_settings()->moveDictionary(transl_info, dict_info,
                                                 enabled);
}

void TranslatorsSettingsView::addUserDictionary() {
  bool ok;
  QString dict_filename =
      QInputDialog::getText(this, tr("New dictionary dialog"), tr("Dict name:"),
                            QLineEdit::Normal, QString(), &ok);

  if (ok && !dict_filename.isEmpty()) {
    fs::path path(dict_filename.toStdString());
    path = path.stem();
    path.replace_extension(".txt");
    path = Y_USER_DICTS_PATH / path;

    bool is_in = model_->translators_settings()->isIn(
        dict::USER_TRANSLATOR_INFO, path.string());
    if (is_in) {
      auto btn = QMessageBox::warning(
          this, tr("Warning"),
          QString(tr("%1 is already exists. "
                     "This operation will completely erase "
                     "data in it. Are you sure?")
                      .arg(QString::fromStdString(path.string()))),
          QMessageBox::Ok | QMessageBox::Cancel);
      if (btn == QMessageBox::Ok) {
        emit needAddUserDictionary(QString::fromStdString(path.string()));
      }
    } else {
      emit needAddUserDictionary(QString::fromStdString(path.string()));
    }
  } else {
    QMessageBox::critical(this, tr("Error"),
                          QString(tr("Something went wrong.")),
                          QMessageBox::Ok);
  }
}

void TranslatorsSettingsView::openUserDictionary(const QString &filename) {
  QDesktopServices::openUrl(QUrl::fromLocalFile(filename));
}

void TranslatorsSettingsView::showMenu(QPoint pos) {
  menu_->move(mapToGlobal(pos));
  menu_->show();
}

QVector<QTableWidgetItem *> TranslatorsSettingsView::makeRow(
    const std::string &transl_info, const std::string &dict_info,
    bool enabled) {
  QVector<QTableWidgetItem *> res;
  res.append(new QTableWidgetItem(QString::fromStdString(transl_info)));
  res.back()->setFlags(Qt::ItemIsEnabled);
  res.append(new QTableWidgetItem(QString::fromStdString(dict_info)));
  res.back()->setFlags(Qt::ItemIsEnabled);
  res.append(new QTableWidgetItem(""));
  res.back()->setFlags(Qt::ItemIsEnabled | Qt::ItemIsUserCheckable);
  res.back()->setCheckState(enabled ? Qt::Checked : Qt::Unchecked);
  return res;
}

void TranslatorsSettingsView::initButtons() {
  new_dict_btn_ = new QPushButton(tr("New dict"));
  connect(new_dict_btn_, &QPushButton::clicked, this,
          &TranslatorsSettingsView::addUserDictionary);

  reload_btn_ = new QPushButton(tr("Reload"));
}

void TranslatorsSettingsView::initTable() {
  table_ = new QTableWidget(this);
  connect(table_, &QTableWidget::itemClicked, this,
          &TranslatorsSettingsView::tableItemClicked);
}

void TranslatorsSettingsView::initLayout() {
  auto vbox = new QVBoxLayout(this);

  vbox->addWidget(table_);

  auto button_hbox = new QHBoxLayout(this);
  vbox->addLayout(button_hbox);

  button_hbox->addWidget(new_dict_btn_);
  button_hbox->addWidget(reload_btn_);
}

void TranslatorsSettingsView::initMenu() {
  menu_ = new QMenu(this);

  auto open_dict_ = new QAction(tr("Open dictionary"));
  //  connect(open_dict_, &QAction::triggered, this,
  //          &TextView::needShowTranslatorsSettingsView);
  menu_->addAction(open_dict_);

  setContextMenuPolicy(Qt::CustomContextMenu);
  connect(this, &TranslatorsSettingsView::customContextMenuRequested, this,
          &TranslatorsSettingsView::showMenu);
}
