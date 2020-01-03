#ifndef TRANSLATOR_H
#define TRANSLATOR_H

#include <filesystem>
#include <future>
#include <memory>
#include <string>
#include <vector>

#include "dictionary.h"

namespace fs = std::filesystem;

namespace dict {

class TranslateResult {};

class Translator {
 public:
  virtual ~Translator();

  TranslateResult translate(const std::wstring& wstr);
  TranslateResult translate(const std::string& str);

 protected:
  virtual TranslateResult doTranslate(const std::string& str) = 0;
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
  TranslateResult doTranslate(const std::string& str) override;
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
  TranslateResult doTranslate(const std::string& str) override;
};

}  // namespace dict

#endif  // TRANSLATOR_H
