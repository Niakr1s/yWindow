#ifndef TEXTCONTROLLER_H
#define TEXTCONTROLLER_H

#include <QObject>
#include <QPoint>
#include <QString>
#include <filesystem>
#include <string>

namespace fs = std::filesystem;

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

  void needTranslate(std::pair<int, int> model_pos, bool with_shift);
  void needReloadDicts();
  void needShowTranslatorsSettingsView();
  void needAddUserDictionary(const QString& filename);
  void needMoveTranslationView(QPoint point);

 public slots:
  void charHovered(std::pair<int, int> model_pos, QPoint point,
                   bool with_shift);

 protected:
  TextModel* model_;
};

#endif  // TEXTCONTROLLER_H
