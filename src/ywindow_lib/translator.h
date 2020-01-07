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

  TranslationResult translate(const std::wstring& wstr, bool all);
  TranslationResult translate(const std::string& str, bool all);

 protected:
  virtual TranslationResult doTranslate(const std::string& str, bool all) = 0;
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

  void prepareDictionaries() final;
  TranslationResult doTranslate(const std::string& str, bool all) override;

 private:
  TranslationChunk* translateAnyOfSubStr(const std::string& str, size_t begin,
                                         size_t count);
  TranslationChunk* translateFullSubStr(const std::string& str, size_t begin,
                                        size_t count);
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
  TranslationResult doTranslate(const std::string& str, bool all) override;

 private:
  Translator* next_translator_;
};

}  // namespace dict

#endif  // TRANSLATOR_H
