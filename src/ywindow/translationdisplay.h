#ifndef TRANSLATIONDISPLAY_H
#define TRANSLATIONDISPLAY_H

#include <QTextEdit>

class TranslationDisplay : public QTextEdit {
 public:
  TranslationDisplay(QWidget* widget = nullptr);

  void append(const std::string& str);
};

#endif  // TRANSLATIONDISPLAY_H
