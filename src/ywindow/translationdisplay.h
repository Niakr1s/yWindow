#ifndef TRANSLATIONDISPLAY_H
#define TRANSLATIONDISPLAY_H

#include "translationview.h"

class TranslationDisplay : public TranslationView {
 public:
  TranslationDisplay(QWidget* widget = nullptr);

  void append(const std::string& str);

 protected:
  void doDisplayTranslation(const dict::TranslationChunk& translation,
                            QPoint pos) override;
};

#endif  // TRANSLATIONDISPLAY_H
