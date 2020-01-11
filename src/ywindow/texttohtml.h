#ifndef TEXTTOHTML_H
#define TEXTTOHTML_H

#include <QString>
#include <QStringList>
#include <initializer_list>
#include <string>

class TextToHtml {
 public:
  TextToHtml(QStringList colors, QString highlight_color);

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
  QStringList colors_;
  int current_color_;
  QString highlight_color_;
  QString result_;

  void addChunkWithColor(const QString& str, const QString& color);
};

#endif  // TEXTTOHTML_H
