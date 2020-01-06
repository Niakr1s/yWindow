#include "ystyler.h"

#include <QFont>
#include <QPalette>
#include <QTextBrowser>
#include <QTextEdit>

YStyler::YStyler() {}

void YStyler::appendStyle(QTextBrowser *text_edit, int pixel_size) {
  text_edit->setAlignment(Qt::AlignTop | Qt::AlignLeft);
  text_edit->setWordWrapMode(QTextOption::WordWrap);
  text_edit->setReadOnly(true);
  text_edit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  text_edit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  text_edit->viewport()->setCursor(QCursor(Qt::ArrowCursor));
  text_edit->setFrameStyle(QFrame::NoFrame);
  text_edit->setMouseTracking(true);
  text_edit->setWindowFlags(Qt::FramelessWindowHint);
  text_edit->setWindowOpacity(0.8);
  text_edit->setStyleSheet("background: black; color: lightGray;");

  QFont f = text_edit->font();
  f.setPixelSize(pixel_size);
  text_edit->setFont(f);

  auto pal = text_edit->palette();
  pal.setColor(QPalette::Text, Qt::lightGray);
  pal.setColor(QPalette::Base, Qt::black);
  pal.setColor(QPalette::Window, Qt::black);
  text_edit->setPalette(pal);
}
