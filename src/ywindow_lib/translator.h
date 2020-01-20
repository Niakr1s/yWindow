#ifndef TRANSLATOR_H
#define TRANSLATOR_H

#include <filesystem>
#include <fstream>
#include <future>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include "dictionary.h"
#include "translationresult.h"
#include "translatorssettings.h"

namespace fs = std::filesystem;

namespace dict {

extern const std::string YOMI_TRANSLATOR_INFO;
extern const std::string DEINFLECT_TRANSLATOR_INFO;
extern const std::string USER_TRANSLATOR_INFO;
extern const std::string CHAIN_TRANSLATOR_INFO;

class Translator {
  friend class ChainTranslator;

 public:
  virtual ~Translator();

  TranslationResult translate(const std::wstring& wstr);
  TranslationResult translate(const std::string& str);
  void reload();

  void setTranslatorsSettings(
      std::shared_ptr<TranslatorsSettings> translators_settings);

  virtual std::string info() const = 0;

 protected:
  virtual TranslationResult doTranslate(const std::string& str) = 0;
  virtual void doPrepareDictionaries() = 0;
  virtual void doSetTranslatorsSettings(
      std::shared_ptr<TranslatorsSettings> translators_settings) = 0;
  virtual void doUpdateTranslatorsSettings() = 0;
  virtual void doReload() = 0;

  static size_t MAX_CHUNK_SIZE;

  std::mutex mutex_;
};

template <class Dict>
class DirectoryTranslator : public Translator {
 public:
  DirectoryTranslator(const fs::path& root_dir,
                      Translator* deinflector = nullptr);

  void setDeinflector(Translator* deinflector);

 protected:
  // in children provide dicts via dicts_futures_ or dicts_
  std::map<fs::path, std::unique_ptr<Dictionary>> dicts_;
  std::map<fs::path, std::future<Dictionary*>> dicts_futures_;
  std::unique_ptr<Translator> deinflector_;
  std::shared_ptr<TranslatorsSettings> translators_settings_;
  fs::path root_dir_;
  std::map<fs::path, fs::file_time_type> paths_;

  /*
  prefer to use this function to load future,
  because it's easy to forget update last_write_time
  */
  void addFutureAndUpdateLastWriteTime(const fs::path& path);

  CardPtrs queryAllNonDisabledDicts(const std::string& str);

  void doPrepareDictionaries() override;
  void doSetTranslatorsSettings(
      std::shared_ptr<TranslatorsSettings> translators_settings) override;
  void doUpdateTranslatorsSettings() override;
  void doReload() override;

  // template main function, use it with TranslatedChunk or TranslatedChunkFinal
  template <class TranslatedChunk_T>
  TranslationResult doTranslateAll(const std::string& str);

 private:
  // second arg of pair is num of first full utf8 symbols translated
  // equal to num of first translated chunks() in TranslationResult{str}
  template <class TranslatedChunk_T>
  std::pair<TranslationChunkPtr, int> doTranslateFirstOf(
      const std::string& str);

  template <class TranslatedChunk_T>
  TranslationChunkPtr doTranslateFullStr(const std::string& str);

  template <class TranslatedChunk_T>
  TranslationChunkPtr doDeinflectAndTranslateFullStr(const std::string& str);

  void futuresToDicts();
};

class YomiTranslator : public DirectoryTranslator<YomiDictionary> {
 public:
  YomiTranslator(const fs::path& root_dir, Translator* deinflector = nullptr);

  std::string info() const override { return YOMI_TRANSLATOR_INFO; }

 protected:
  TranslationResult doTranslate(const std::string& str) override;
};

// Use only as inner of DictionaryTranslator
class DeinflectTranslator : public DirectoryTranslator<DeinflectDictionary> {
 public:
  DeinflectTranslator(const fs::path& root_dir);

  std::string info() const override { return DEINFLECT_TRANSLATOR_INFO; }

 protected:
  TranslationResult doTranslate(const std::string& str) override;
};

class UserTranslator : public DirectoryTranslator<UserDictionary> {
 public:
  UserTranslator(const fs::path& root_dir);

  std::string info() const override { return USER_TRANSLATOR_INFO; }

 protected:
  TranslationResult doTranslate(const std::string& str) override;
};

// Don't use DeinflectTranslator here, use it as inner of DictionaryTranslator
class ChainTranslator : public Translator {
 public:
  ChainTranslator();
  ChainTranslator(std::initializer_list<Translator*> translators);

  std::string info() const override { return CHAIN_TRANSLATOR_INFO; }

  void addTranslator(Translator* transl);
  void popTranslator();

  void doReload() override;

 protected:
  TranslationResult doTranslate(const std::string& str) override;
  void doPrepareDictionaries() override;
  void doUpdateTranslatorsSettings() override;
  void doSetTranslatorsSettings(
      std::shared_ptr<TranslatorsSettings> translators_settings) override;

 private:
  std::vector<std::unique_ptr<Translator>> translators_;
};

}  // namespace dict

#endif  // TRANSLATOR_H
