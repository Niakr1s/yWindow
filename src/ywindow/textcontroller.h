#ifndef TEXTCONTROLLER_H
#define TEXTCONTROLLER_H

#include <QObject>
#include <QPoint>
#include <QString>
#include <string>

class TextModel;

class TextController : public QObject {
  Q_OBJECT

 public:
  TextController(QObject* parent = nullptr);

  void addText(const std::string& str);
  void addText(const std::wstring& str);
  void addText(const QString& str);

  void setModel(TextModel* model);

 signals:
  void newText(QString);
  void charHovered(std::pair<int, int> model_pos, QPoint point,
                   bool with_shift);

 protected:
  TextModel* model_;
};

#endif  // TEXTCONTROLLER_H
