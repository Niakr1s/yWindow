#ifndef TRANSLATOR_H
#define TRANSLATOR_H

#include <filesystem>
#include <fstream>
#include <future>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include "dictionary.h"
#include "translationresult.h"

namespace fs = std::filesystem;

namespace dict {

class Translator;

struct DictionaryState {
  std::set<std::string> ordered, unordered, disabled;
};

class TranslatorsState {
 public:
  TranslatorsState(const fs::path& json_file);

  void updateFromFS();

  bool isInOrdered(const std::string& translator_info,
                   const std::string& dictionary_info);
  bool isInUnordered(const std::string& translator_info,
                     const std::string& dictionary_info);
  bool isInDisabled(const std::string& translator_info,
                    const std::string& dictionary_info);
  bool isIn(const std::string& translator_info,
            const std::string& dictionary_info);
  bool notIn(const std::string& translator_info,
             const std::string& dictionary_info);

  // don't forget to saveJson in the end
  void addUnorderedDictionary(const std::string& translator_info,
                              const std::string& dictionary_info);
  void saveJson();

  const std::map<std::string, DictionaryState>& states() const;

 private:
  fs::path json_file_;
  fs::file_time_type last_write_time_;
  std::map<std::string, DictionaryState> states_;

  const std::string ORDERED = "ordered";
  const std::string UNORDERED = "unordered";
  const std::string DISABLED = "disabled";

  void loadJson();
  bool fileChanged();  // changes last_write_time_ !!

  bool isIn(const std::string& dictionary_info,
            const std::set<std::string>& vec);
};

class Translator {
 public:
  virtual ~Translator();

  TranslationResult translate(const std::wstring& wstr);
  TranslationResult translate(const std::string& str);

  virtual void setTranslatorsState(
      std::shared_ptr<TranslatorsState> translators_state) = 0;

  virtual std::string info() const = 0;

 protected:
  virtual TranslationResult doTranslate(const std::string& str) = 0;
  virtual void prepareDictionaries();

  static size_t MAX_CHUNK_SIZE;
};

class DictionaryTranslator : public Translator {
 public:
  DictionaryTranslator(Translator* deinflector = nullptr);

  void addDict(Dictionary* dict);
  void setDeinflector(Translator* deinflector);
  void setTranslatorsState(
      std::shared_ptr<TranslatorsState> translators_state) override;

 protected:
  // in children provide dicts via dicts_futures_ or dicts_
  std::vector<std::unique_ptr<Dictionary>> dicts_;
  std::vector<std::future<Dictionary*>> dicts_futures_;
  std::unique_ptr<Translator> deinflector_;
  std::shared_ptr<TranslatorsState> translators_state_;

  CardPtrs queryAllDicts(const std::string& str);

  void prepareDictionaries() final;

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
};

class YomiTranslator : public DictionaryTranslator {
 public:
  YomiTranslator(const fs::path& root_dir, Translator* deinflector = nullptr);
  YomiTranslator(std::initializer_list<fs::path> dicts_dirs,
                 Translator* deinflector = nullptr);

  std::string info() const override { return "YomiTranslator"; }

 protected:
  TranslationResult doTranslate(const std::string& str) override;
};

// Use only as inner of DictionaryTranslator
class DeinflectTranslator : public DictionaryTranslator {
 public:
  DeinflectTranslator(const fs::path& file);

  std::string info() const override { return "DeinflectTranslator"; }

 protected:
  TranslationResult doTranslate(const std::string& str) override;
};

class UserTranslator : public DictionaryTranslator {
 public:
  UserTranslator(const fs::path& file);

  std::string info() const override { return "UserTranslator"; }

 protected:
  TranslationResult doTranslate(const std::string& str);
};

// Don't use DeinflectTranslator here, use it as inner of DictionaryTranslator
class ChainTranslator : public Translator {
 public:
  ChainTranslator();
  ChainTranslator(std::initializer_list<Translator*> translators);

  std::string info() const override { return "ChainTranslator"; }

  void setTranslatorsState(
      std::shared_ptr<TranslatorsState> translators_state) override;

  void addTranslator(Translator* transl);
  void popTranslator();

 protected:
  TranslationResult doTranslate(const std::string& str) override;

 private:
  std::vector<std::unique_ptr<Translator>> translators_;
};

}  // namespace dict

#endif  // TRANSLATOR_H
