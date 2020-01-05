#ifndef YSTYLER_H
#define YSTYLER_H

class QTextEdit;

class YStyler {
 public:
  YStyler();

  void appendStyle(QTextEdit* text_edit, int pixel_size);
};

#endif  // YSTYLER_H
