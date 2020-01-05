#ifndef TEXTVIEW_H
#define TEXTVIEW_H

#include <QObject>
#include <QString>

#include "card.h"

class TextController;
class TextModel;

class TextView : public QObject {
  Q_OBJECT

 public:
  TextView(QObject* parent = nullptr);
  virtual ~TextView();

  void setController(TextController* controller);
  void setModel(TextModel* model);

  virtual int fontHeight() = 0;

 signals:
  void mouseHovered(int pos);

 public slots:
  void displayText();
  void displayTranslation(dict::CardPtrList translation);

 protected:
  TextController* controller_;
  TextModel* model_;

  virtual void doDisplayText() = 0;
  virtual void doDisplayTranslation(const dict::CardPtrList& translation) = 0;
};

#endif  // TEXTVIEW_H