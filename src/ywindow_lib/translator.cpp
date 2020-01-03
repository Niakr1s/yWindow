#include "translator.h"

#include "dictionary.h"
#include "loader.h"
#include "stringconvert.h"

dict::Translator::~Translator() {}

dict::TranslateResult dict::Translator::translate(const std::wstring &wstr) {
  std::string str = WideStringToString(wstr);
  return translate(str);
}

dict::TranslateResult dict::Translator::translate(const std::string &str) {
  return doTranslate(str);
}

dict::YomiTranslator::YomiTranslator(const fs::path &root_dir) {
  for (auto &path : fs::directory_iterator(root_dir)) {
    if (!path.is_directory()) {
      continue;
    }
    dicts_.push_back(Loader::loadFromDirectory<YomiDictionary>(path));
  }
}

dict::YomiTranslator::YomiTranslator(
    std::initializer_list<fs::path> dicts_dirs) {
  for (auto &dir : dicts_dirs) {
    dicts_.push_back(Loader::loadFromDirectory<YomiDictionary>(dir));
  }
}

dict::TranslateResult dict::YomiTranslator::doTranslate(
    const std::string &str) {
  return TranslateResult{};
}
