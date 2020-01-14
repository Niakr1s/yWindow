#ifndef TEXTMODEL_H
#define TEXTMODEL_H

#include <QObject>
#include <QPoint>
#include <deque>

#include "card.h"
#include "translationresult.h"
#include "translator.h"

class TextModel : public QObject {
  Q_OBJECT

 public:
  TextModel(QObject* parent = nullptr);
  virtual ~TextModel();

  QStringList toHtml();
  QStringList toPlainText();

  void setTranslatorsSettings(
      std::shared_ptr<dict::TranslatorsSettings> translators_settings);

  virtual bool isOnlyHovered() const = 0;

 signals:
  void textChanged();
  void gotTranslation(dict::TranslationChunkPtr, QPoint point);
  void gotTranslationLength(int length);
  void cancelTranslation();

 public slots:
  void translate(std::pair<int, int> pos, QPoint point, bool with_shift);
  void addText(QString text);

  std::shared_ptr<dict::TranslatorsSettings> translators_settings() const;

 protected:
  std::pair<int, int> last_pos_ = {-1, -1};
  int current_pos_ = -1;
  std::shared_ptr<dict::TranslatorsSettings> translators_settings_;
  bool translators_settings_applied_ = false;

  virtual QStringList doToHtml() = 0;
  virtual QStringList doToPlainText() = 0;
  virtual dict::TranslationChunkPtr doTranslate(std::pair<int, int> pos) = 0;
  virtual dict::TranslationChunkPtr doTranslateFromPos(
      std::pair<int, int> pos) = 0;
  virtual void doAddText(const QString& text) = 0;
  virtual void prepareTranslator() = 0;
};

class DefaultModel : public TextModel {
 public:
  DefaultModel(dict::Translator* translator, QObject* parent = nullptr);

  bool isOnlyHovered() const override;

  std::shared_ptr<dict::TranslatorsSettings> translators_settings() const;

 protected:
  QStringList doToHtml() override;
  QStringList doToPlainText() override;
  dict::TranslationChunkPtr doTranslate(std::pair<int, int> pos) override;
  dict::TranslationChunkPtr doTranslateFromPos(
      std::pair<int, int> pos) override;
  void doAddText(const QString& text) override;
  void prepareTranslator() override;

 private:
  const size_t max_size_ = 10;
  std::deque<dict::TranslationResult> text_;
  std::unique_ptr<dict::Translator> translator_;
};

#endif  // TEXTMODEL_H
