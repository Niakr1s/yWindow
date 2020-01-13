#include "texttohtml.h"

TextToHtml::TextToHtml() {
  result_ = R"(<head>
    <link rel="stylesheet" type="text/css" href="ytranslation.css">
</head>)";
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

void TextToHtml::addChunkHighlighted(const std::string& str) {
  return addChunkHighlighted(QString::fromStdString(str));
}

void TextToHtml::addChunk(const QString& str) {
  return addChunkWithColor(str, colors_[current_color_]);
}

void TextToHtml::addChunkNoColor(const QString& str) {
  return addChunkWithColor(str, "");
}

void TextToHtml::addChunkHighlighted(const QString& str) {
  return addChunkWithColor(str, "text_highlighted_chunk");
}

QString TextToHtml::result() const { return result_; }

void TextToHtml::clear() { result_.clear(); }

void TextToHtml::addChunkWithColor(const QString& str,
                                   const QString& class_name) {
  result_.append(QString(R"***(<span class="text %1">%2</span>)***")
                     .arg(class_name)
                     .arg(str));
}
