#ifndef YSTYLER_H
#define YSTYLER_H

class QTextEdit;
class QTextBrowser;

class YStyler {
 public:
  YStyler();

  void appendStyle(QTextBrowser* text_edit, int pixel_size);
};

#endif  // YSTYLER_H
