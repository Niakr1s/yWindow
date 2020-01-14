#ifndef TEXTVIEW_H
#define TEXTVIEW_H

#include <QMenu>
#include <QString>
#include <QTextBrowser>

#include "card.h"
#include "hoversyntaxhighlighter.h"
#include "translationresult.h"
#include "translatorssettingsview.h"

class TextController;
class TextModel;

class TextView : public QTextBrowser {
  Q_OBJECT

 public:
  TextView(QWidget* parent = nullptr);
  virtual ~TextView();

  void setController(TextController* controller);
  void setModel(TextModel* model);

 signals:
  void charHovered(std::pair<int, int> model_pos, QPoint point,
                   bool with_shift);

 public slots:
  void displayText();
  virtual void highlightTranslated(int length) = 0;

 protected:
  TextController* controller_;
  TextModel* model_;

  virtual void doDisplayText() = 0;
  virtual void doSetModel(TextModel* model) = 0;
};

class DefaultTextView : public TextView {
  Q_OBJECT

 public:
  DefaultTextView(QWidget* parent = nullptr);

  struct LastHovered {
    std::pair<int, int> model_pos = {-1, -1};
    std::pair<int, int> inner_pos = {-1, -1};
    int inner_col = -1;
  };

  class WrongPosException : public std::exception {
    using std::exception::exception;
  };

 public slots:
  void highlightTranslated(int length) override;
  void showMenu(QPoint pos);

 private:
  const QString anchor_ = "last";

 protected:
  void doDisplayText() override;
  void doSetModel(TextModel* model) override;

  void mouseMoveEvent(QMouseEvent* event) override;
  void leaveEvent(QEvent* event) override;

 private:
  QStringList current_text_;
  LastHovered last_hovered_;
  HoverSyntaxHighlighter* highlighter_;

  std::pair<int, int> innerColToModelPos(int pos);

  void emitCharHovered(std::pair<int, int> model_pos, QPoint point,
                       bool with_shift = false);
  void initMenu();

  QMenu* menu_;
  TranslatorsSettingsView* translators_settings_view_;
};

#endif  // TEXTVIEW_H
