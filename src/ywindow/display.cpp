#include "display.h"

Display::Display(QWidget* parent) : QTextEdit(parent) {
  setAlignment(Qt::AlignTop | Qt::AlignLeft);
  setWordWrapMode(QTextOption::WordWrap);
  setReadOnly(true);
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  viewport()->setCursor(QCursor(Qt::ArrowCursor));
  setFrameStyle(QFrame::NoFrame);

  QFont f = font();
  f.setPixelSize(40);
  setFont(f);

  auto pal = palette();
  pal.setColor(QPalette::Text, Qt::lightGray);
  pal.setColor(QPalette::Base, Qt::black);
  pal.setColor(QPalette::Window, Qt::black);
  setPalette(pal);
}

void Display::insertText(QString text) {
  text = text.trimmed();
  if (text.isEmpty()) {
    return;
  }

  text = QString("<a name=\"last\">%1</a>").arg(text);

  auto trimmed = toPlainText().trimmed();
  setPlainText(trimmed);
  moveCursor(QTextCursor::End);
  if (!trimmed.isEmpty()) {
    insertEndl(1);
  }

  insertHtml(text);

  insertEndl(height() / (font().pixelSize()));
  scrollToAnchor("last");
}

void Display::insertEndl(int count) {
  while (count--) {
    insertPlainText("\n");
  }
}
