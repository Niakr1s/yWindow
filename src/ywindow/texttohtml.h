#ifndef TEXTTOHTML_H
#define TEXTTOHTML_H

#include <QString>
#include <QStringList>
#include <initializer_list>
#include <string>

class TextToHtml {
 public:
  TextToHtml();

  void nextColor();

  void addChunk(const std::string& str);
  void addChunkNoColor(const std::string& str);
  void addChunkHighlighted(const std::string& str);

  void addChunk(const QString& str);
  void addChunkNoColor(const QString& str);
  void addChunkHighlighted(const QString& str);

  QString result() const;
  void clear();

 private:
  QStringList colors_{"text_chunk1", "text_chunk2"};
  int current_color_ = 0;
  QString result_;

  void addChunkWithColor(const QString& str, const QString& class_name);
};

#endif  // TEXTTOHTML_H
