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

dict::YomiTranslator::YomiTranslator(const fs::path &root_dir)
    : DictionaryTranslator() {
  for (auto &path : fs::directory_iterator(root_dir)) {
    if (!path.is_directory()) {
      continue;
    }
    dicts_futures_.push_back(Loader::loadFromFS<YomiDictionary>(path));
  }
}

dict::YomiTranslator::YomiTranslator(std::initializer_list<fs::path> dicts_dirs)
    : DictionaryTranslator() {
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
    res.chunks().push_back(translateAnyOfSubStr(str, 0, str.size()));
  } else {
    for (size_t i = 0, i_max = str.size(); i != i_max; ++i) {
      auto chunk = translateAnyOfSubStr(str, i, str.size());
      if (chunk.translated()) {
        res.chunks().push_back(chunk);
        i = chunk.orig_end() + 1;
      }
    }
  }
  return res;
}

dict::TranslationChunk dict::DictionaryTranslator::translateAnyOfSubStr(
    const std::string &str, size_t begin, size_t count) {
  for (size_t i = std::min(count, MAX_CHUNK_SIZE); i != 0; --i) {
    auto chunk = translateFullSubStr(str, begin, i - begin);
    if (chunk.translated()) return chunk;
  }
  return TranslationChunk{str.substr(begin, count), begin, begin + count - 1};
}

dict::TranslationChunk dict::DictionaryTranslator::translateFullSubStr(
    const std::string &str, size_t begin, size_t count) {
  string str_chunk = str.substr(begin, count);
  TranslationChunk chunk{str_chunk, begin, begin + count - 1};
  for (auto &dict : dicts_) {
    auto query = dict->query(str_chunk);
    for (auto &card : query) {
      chunk.translations().push_back(card);
    }
  }
  if (!chunk.translated()) return chunk;
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

dict::DictionaryTranslator::DictionaryTranslator() {}

void dict::DictionaryTranslator::addDict(dict::Dictionary *dict) {
  dicts_.push_back(std::unique_ptr<Dictionary>(dict));
}
