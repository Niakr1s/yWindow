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
    dicts_futures_.push_back(Loader::loadFromFS<YomiDictionary>(path));
  }
}

dict::YomiTranslator::YomiTranslator(
    std::initializer_list<fs::path> dicts_dirs) {
  for (auto &dir : dicts_dirs) {
    dicts_futures_.push_back(Loader::loadFromFS<YomiDictionary>(dir));
  }
}

void dict::YomiTranslator::futuresToDicts() {
  while (!dicts_futures_.empty()) {
    try {
      dicts_.push_back(
          std::unique_ptr<Dictionary>(dicts_futures_.back().get()));
    } catch (...) {
    }
    dicts_futures_.pop_back();
  }
}

dict::TranslateResult dict::YomiTranslator::doTranslate(
    const std::string &str) {
  if (!dicts_futures_.empty()) {
    futuresToDicts();
  }
  // TODO
  return TranslateResult{};
}

dict::TranslatorDecorator::TranslatorDecorator(dict::Translator *translator)
    : next_translator_(translator) {}

dict::DeinflectTranslator::DeinflectTranslator(dict::Translator *translator)
    : TranslatorDecorator(translator) {}

dict::TranslateResult dict::DeinflectTranslator::doTranslate(
    const std::string &str) {
  // TODO
  return TranslateResult{};
}
