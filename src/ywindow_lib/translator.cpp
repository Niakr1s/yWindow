#include "translator.h"

#include <algorithm>

#include "dictionary.h"
#include "loader.h"
#include "stringconvert.h"

dict::Translator::~Translator() {}

dict::TranslationResult dict::Translator::translate(const std::wstring &wstr,
                                                    bool all) {
  std::string str = WideStringToString(wstr);
  return translate(str, all);
}

dict::TranslationResult dict::Translator::translate(const std::string &str,
                                                    bool all) {
  auto res = doTranslate(str, all);
  res.normalize();
  return res;
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

dict::TranslationResult dict::YomiTranslator::doTranslate(
    const std::string &str, bool all) {
  if (!dicts_futures_.empty()) {
    futuresToDicts();
  }
  TranslationResult res{str};
  if (!all) {
    const size_t begin = 0;
    for (size_t i = std::min(str.size(), MAX_CHUNK_SIZE); i != 0; --i) {
      auto chunk = translateSubStr(str, begin, i - begin, false);
      if (chunk.translated()) {
        chunk = translateSubStr(str, begin, i - begin, true);
        res.chunks().push_back(chunk);
      }
    }
  }
  // TODO
  return res;
}

dict::TranslationChunk dict::YomiTranslator::translateSubStr(
    const std::string &str, size_t begin, size_t count, bool all) {
  string str_chunk = str.substr(begin, count);
  TranslationChunk chunk{str_chunk, begin, begin + count - 1};
  for (auto &dict : dicts_) {
    auto query = dict->query(str_chunk);
    for (auto &card : query) {
      chunk.translations().push_back(card);
    }
  }
  if (!all) return chunk;
  while (--count != 0) {
    str_chunk = str.substr(begin, count);
    for (auto &dict : dicts_) {
      auto query = dict->query(str_chunk);
      for (auto &card : query) {
        chunk.translations().push_back(card);
      }
    }
  }
  return chunk;
}

dict::TranslatorDecorator::TranslatorDecorator(dict::Translator *translator)
    : next_translator_(translator) {}

dict::DeinflectTranslator::DeinflectTranslator(dict::Translator *translator)
    : TranslatorDecorator(translator) {}

dict::TranslationResult dict::DeinflectTranslator::doTranslate(
    const std::string &str, bool all) {
  // TODO
  return TranslationResult{str};
}

size_t dict::Translator::MAX_CHUNK_SIZE = 30;
