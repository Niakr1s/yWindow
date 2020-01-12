#ifndef YSTYLER_H
#define YSTYLER_H

class QTextEdit;
class QTextBrowser;

class YStyler {
 public:
  YStyler();

  void appendStyleTranslationView(QTextBrowser* text_edit, int pixel_size);
  void appendStyleTextView(QTextBrowser* text_edit, int pixel_size);

 private:
  void appendStyleCommonView(QTextBrowser* text_edit);
  void setPixelSize(QTextBrowser* text_edit, int pixel_size);
};

#endif  // YSTYLER_H
