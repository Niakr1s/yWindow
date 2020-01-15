#include "translatorssettingsview.h"

#include <QDebug>
#include <QHeaderView>
#include <QScrollBar>
#include <QVBoxLayout>

TranslatorsSettingsView::TranslatorsSettingsView(QWidget *parent)
    : QWidget(parent) {
  auto vbox = new QVBoxLayout(this);

  table_ = new QTableWidget(this);
  vbox->addWidget(table_);

  connect(table_, &QTableWidget::itemClicked, this,
          &TranslatorsSettingsView::tableItemClicked);
}

void TranslatorsSettingsView::setTextModel(TextModel *model) {
  model_ = model;
  connect(model_, &TextModel::translatorSettingsChanged, this,
          &TranslatorsSettingsView::redraw);
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
