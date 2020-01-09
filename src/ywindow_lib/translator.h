#ifndef TRANSLATOR_H
#define TRANSLATOR_H

#include <filesystem>
#include <future>
#include <memory>
#include <string>
#include <vector>

#include "dictionary.h"
#include "translationresult.h"

namespace fs = std::filesystem;

namespace dict {

class Translator {
 public:
  virtual ~Translator();

  TranslationResult translate(const std::wstring& wstr);
  TranslationResult translate(const std::string& str);

 protected:
  virtual TranslationResult doTranslate(const std::string& str) = 0;
  virtual void prepareDictionaries() = 0;

  static size_t MAX_CHUNK_SIZE;
};

class DictionaryTranslator : public Translator {
 public:
  DictionaryTranslator();

  void addDict(Dictionary* dict);

 protected:
  // in children provide dicts via dicts_futures_ or dicts_
  std::vector<std::unique_ptr<Dictionary>> dicts_;
  std::vector<std::future<Dictionary*>> dicts_futures_;

  CardPtrMultiMap queryAllDicts(const std::string& str);

  void prepareDictionaries() final;
  TranslationResult doTranslate(const std::string& str) override;

  TranslationResult doTranslateAll(const std::string& str);

  // second arg of pair is num of first full utf8 symbols translated
  // equal to num of first translated chunks() in TranslationResult{str}
  std::pair<TranslationChunkPtr, int> doTranslateFirstOf(
      const std::string& str);

  TranslationChunkPtr doTranslateFullStr(const std::string& str);
};

class YomiTranslator : public DictionaryTranslator {
 public:
  YomiTranslator(const fs::path& root_dir);
  YomiTranslator(std::initializer_list<fs::path> dicts_dirs);
};

class DeinflectTranslator : public DictionaryTranslator {
 public:
  DeinflectTranslator(const fs::path& file, Translator* next_translator);

 protected:
  TranslationResult doTranslate(const std::string& str) override;

 private:
  Translator* next_translator_;
};

}  // namespace dict

#endif  // TRANSLATOR_H
