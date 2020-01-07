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

  static size_t MAX_CHUNK_SIZE;
};

class DictionaryTranslator : public Translator {
 public:
  DictionaryTranslator();

  void addDict(Dictionary* dict);

 protected:
  std::vector<std::unique_ptr<Dictionary>> dicts_;

  // called before actual translation, use it for initializing dicts_ variable
  virtual void prepareDictionaries() = 0;

  TranslationResult doTranslate(const std::string& str, bool all) override;

  TranslationChunk translateAnyOfSubStr(const std::string& str, size_t begin,
                                        size_t count);
  TranslationChunk translateFullSubStr(const std::string& str, size_t begin,
                                       size_t count);
};

class YomiTranslator : public DictionaryTranslator {
 public:
  YomiTranslator(const fs::path& root_dir);
  YomiTranslator(std::initializer_list<fs::path> dicts_dirs);

  void prepareDictionaries() override;

 private:
  std::vector<std::future<Dictionary*>> dicts_futures_;
};

class DeinflectTranslator : public DictionaryTranslator {
 public:
  DeinflectTranslator(const fs::path& file, Translator* next_translator);

 protected:
  void prepareDictionaries() override;
  TranslationResult doTranslate(const std::string& str, bool all) override;

 private:
  Translator* next_translator_;
  std::future<Dictionary*> future_;
};

}  // namespace dict

#endif  // TRANSLATOR_H
