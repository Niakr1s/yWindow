#ifndef TRANSLATOR_H
#define TRANSLATOR_H

#include <filesystem>
#include <future>
#include <string>
#include <vector>

namespace fs = std::filesystem;

namespace dict {

class Dictionary;

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
  std::vector<std::future<Dictionary*>> dicts_;

 protected:
  TranslateResult doTranslate(const std::string& str) override;
};

}  // namespace dict

#endif  // TRANSLATOR_H
