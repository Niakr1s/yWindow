#include "texttohtml.h"

TextToHtml::TextToHtml(QStringList colors)
    : colors_(colors), current_color_(0) {
  if (colors.empty()) {
    throw std::invalid_argument("TextToHtml: empty colors");
  }
}

void TextToHtml::nextColor() {
  if (++current_color_ >= colors_.size()) {
    current_color_ = 0;
  }
}

void TextToHtml::addChunk(const std::string& str) {
  return addChunk(QString::fromStdString(str));
}

void TextToHtml::addChunkNoColor(const std::string& str) {
  return addChunkNoColor(QString::fromStdString(str));
}

void TextToHtml::addChunk(const QString& str) {
  return addChunkWithColor(str, colors_[current_color_]);
}

void TextToHtml::addChunkNoColor(const QString& str) {
  result_.append(QString(R"***(<span>%1</span>)***").arg(str));
}

QString TextToHtml::result() const { return result_; }

void TextToHtml::clear() { result_.clear(); }

void TextToHtml::addChunkWithColor(const QString& str, const QString& color) {
  QString style_str = QString(R"***(style="color: %1;")***").arg(color);
  result_.append(
      QString(R"***(<span %1>%2</span>)***").arg(style_str).arg(str));
}
