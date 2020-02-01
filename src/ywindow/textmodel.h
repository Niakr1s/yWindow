#ifndef TEXTMODEL_H
#define TEXTMODEL_H

#include <QObject>
#include <QPoint>
#include <deque>
#include <filesystem>

#include "card.h"
#include "translationresult.h"
#include "translator.h"

namespace fs = std::filesystem;

class TextModel : public QObject {
  Q_OBJECT

 public:
  TextModel(QObject* parent = nullptr);
  virtual ~TextModel();

  QStringList toHtml();
  QStringList toPlainText();

  void setTranslatorsSettings(
      std::shared_ptr<dict::TranslatorsSettings> translators_settings);
  virtual std::shared_ptr<dict::TranslatorsSettings> translators_settings()
      const = 0;

  virtual bool isOnlyHovered() const = 0;

 signals:
  void textChanged();
  void gotTranslation(dict::TranslationChunkPtr);
  void gotTranslationLength(int length);
  void cancelTranslation();
  void startProcessing();
  void endProcessing();
  void translatorSettingsChanged();
  void userDictionaryCreated(const QString& filename);

 public slots:
  void translate(std::pair<int, int> pos, bool with_shift);
  void addText(QString text);
  void reloadDicts();
  void addUserDictionary(const QString& filename);

 protected:
  std::pair<int, int> last_pos_ = {-1, -1};
  int current_pos_ = -1;
  std::shared_ptr<dict::TranslatorsSettings> translators_settings_;

  QString removeWhiteSpaces(const QString& str);

  virtual QStringList doToHtml() = 0;
  virtual QStringList doToPlainText() = 0;
  virtual dict::TranslationChunkPtr doTranslate(std::pair<int, int> pos) = 0;
  virtual dict::TranslationChunkPtr doTranslateFromPos(
      std::pair<int, int> pos) = 0;
  virtual void doAddText(const QString& text) = 0;
  virtual void doSetTranslatorSettings(
      std::shared_ptr<dict::TranslatorsSettings> translators_settings) = 0;
  virtual void doReloadDicts() = 0;
  virtual void doAddUserDictionary(const QString& filename) = 0;
};

class DefaultModel : public TextModel {
 public:
  DefaultModel(dict::Translator* translator, QObject* parent = nullptr);

  bool isOnlyHovered() const override;

  std::shared_ptr<dict::TranslatorsSettings> translators_settings()
      const override;

 protected:
  QStringList doToHtml() override;
  QStringList doToPlainText() override;
  dict::TranslationChunkPtr doTranslate(std::pair<int, int> pos) override;
  dict::TranslationChunkPtr doTranslateFromPos(
      std::pair<int, int> pos) override;
  void doAddText(const QString& text) override;
  void doSetTranslatorSettings(
      std::shared_ptr<dict::TranslatorsSettings> translators_settings) override;
  void doReloadDicts() override;
  void doAddUserDictionary(const QString& filename) override;

 private:
  const size_t max_size_ = 10;
  std::deque<dict::TranslationResult> text_;
  std::unique_ptr<dict::Translator> translator_;
};

#endif  // TEXTMODEL_H
