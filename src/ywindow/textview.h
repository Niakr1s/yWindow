#ifndef TEXTVIEW_H
#define TEXTVIEW_H

#include <QString>
#include <QTextEdit>

#include "card.h"
#include "translationresult.h"

class TextController;
class TextModel;

class TextView : public QTextEdit {
  Q_OBJECT

 public:
  TextView(QWidget* parent = nullptr);
  virtual ~TextView();

  void setController(TextController* controller);
  void setModel(TextModel* model);

  virtual int fontHeight() = 0;

 signals:
  void charHovered(std::pair<int, int> line_and_col);

 public slots:
  void displayText();
  void displayTranslation(dict::TranslationChunk translation);

 protected:
  TextController* controller_;
  TextModel* model_;

  virtual void doDisplayText() = 0;
  virtual void doDisplayTranslation(
      const dict::TranslationChunk& translation) = 0;
};

#endif  // TEXTVIEW_H
