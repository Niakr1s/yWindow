#include "translationdisplay.h"

#include <QDebug>

TranslationDisplay::TranslationDisplay(QWidget* widget)
    : TranslationView(widget) {}

void TranslationDisplay::append(const std::string& str) {
  return QTextEdit::append(QString::fromStdString(str));
}

void TranslationDisplay::doDisplayTranslation(
    const dict::TranslationChunk& translation, QPoint pos) {
  qDebug() << "TranslationDisplay: got pos " << pos << "height " << height();
  pos.setY(pos.y() - height());
  move(pos);

  auto tr = translation.translations();
  qDebug() << "got " << tr.size() << " translations";
  clear();
  for (auto& t : tr) {
    append(t.second->name());
    append(t.second->reading());
    append(t.second->meaning());
    append(t.second->dictionaryInfo());
    append("------------");
  }

  // TODO refactor
  auto sub_tr = translation.subTranslations();
  for (auto& t : sub_tr) {
    append(t.second->name());
    append(t.second->reading());
    append(t.second->meaning());
    append(t.second->dictionaryInfo());
    append("------------");
  }
  show();
}
