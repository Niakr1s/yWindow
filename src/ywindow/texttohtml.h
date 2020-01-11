#ifndef TEXTTOHTML_H
#define TEXTTOHTML_H

#include <QString>
#include <QStringList>
#include <initializer_list>
#include <string>

class TextToHtml {
 public:
  TextToHtml(QStringList colors);

  void nextColor();

  void addChunk(const std::string& str);
  void addChunkNoColor(const std::string& str);

  void addChunk(const QString& str);
  void addChunkNoColor(const QString& str);

  QString result() const;
  void clear();

 private:
  QStringList colors_;
  int current_color_;
  QString result_;

  void addChunkWithColor(const QString& str, const QString& color);
};

#endif  // TEXTTOHTML_H
