#ifndef TEXTMODEL_H
#define TEXTMODEL_H

#include <QObject>

#include "card.h"

class TextModel : public QObject {
  Q_OBJECT

 public:
  TextModel(QObject* parent = nullptr);
  virtual ~TextModel();

  QStringList getText();

 signals:
  void textChanged();
  void gotTranslation(dict::CardPtrList);

 public slots:
  void translate(int pos);
  void addText(QString text);

 protected:
  int current_pos_ = -1;

  virtual QStringList doGetText() = 0;
  virtual dict::CardPtrList doTranslate(int pos) = 0;
  virtual void doAddText(const QString& text) = 0;
};

#endif  // TEXTMODEL_H
