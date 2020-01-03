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

class YomiTranslator : public Translator {
 public:
  YomiTranslator(const fs::path& root_dir);
  YomiTranslator(std::initializer_list<fs::path> dicts_dirs);

 private:
  std::vector<std::future<Dictionary*>> dicts_futures_;
  std::vector<std::unique_ptr<Dictionary>> dicts_;

  void futuresToDicts();

 protected:
  TranslationResult doTranslate(const std::string& str, bool all) override;

  TranslationChunk translateSubStr(const std::string& str, size_t begin,
                                   size_t count, bool all);
};

class TranslatorDecorator : public Translator {
 public:
  TranslatorDecorator(Translator* translator);

 protected:
  Translator* next_translator_;
};

class DeinflectTranslator : public TranslatorDecorator {
 public:
  DeinflectTranslator(Translator* translator);

 protected:
  TranslationResult doTranslate(const std::string& str, bool all) override;
};

}  // namespace dict

#endif  // TRANSLATOR_H
