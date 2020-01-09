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
  prepareDictionaries();
  auto res = doTranslate(str, all);
  //  res.normalize();
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

void dict::DictionaryTranslator::prepareDictionaries() {
  while (!dicts_futures_.empty()) {
    try {
      dicts_.push_back(
          std::unique_ptr<Dictionary>(dicts_futures_.back().get()));
    } catch (...) {
    }
    dicts_futures_.pop_back();
  }
}

dict::TranslationChunk *dict::DictionaryTranslator::translateAnyOfSubStr(
    const std::string &str, size_t begin, size_t count) {
  for (size_t sz = std::min(count, MAX_CHUNK_SIZE); sz != 0; --sz) {
    auto chunk = translateFullSubStr(str, begin, sz);
    if (chunk->translated()) return chunk;
  }
  // TODO
  return new UntranslatedChunk({str.substr(begin, count)});
}

dict::TranslationChunk *dict::DictionaryTranslator::translateFullSubStr(
    const std::string &str, size_t begin, size_t count) {
  std::string str_chunk = str.substr(begin, count);
  auto chunk = new UntranslatedChunk({str_chunk, begin});  // TODO
  //  for (auto &dict : dicts_) {
  //    auto query = dict->query(str_chunk);
  //    for (auto &card : query) {
  //      chunk->translations().insert(card);
  //    }
  //  }
  //  if (!chunk->translated()) return chunk;
  //  while (--count != 0) {
  //    str_chunk = str.substr(begin, count);
  //    for (auto &dict : dicts_) {
  //      auto query = dict->query(str_chunk);
  //      for (auto &card : query) {
  //        chunk->subTranslations().insert(card);
  //      }
  //    }
  //  }
  return chunk;
}

size_t dict::Translator::MAX_CHUNK_SIZE = 30;

dict::DictionaryTranslator::DictionaryTranslator() {}

void dict::DictionaryTranslator::addDict(dict::Dictionary *dict) {
  dicts_.push_back(std::unique_ptr<Dictionary>(dict));
}

dict::TranslationResult dict::DictionaryTranslator::doTranslate(
    const std::string &str, bool all) {
  TranslationResult res{str};
  if (!all) {
    res.chunks().push_back(
        TranslationChunkPtr(translateAnyOfSubStr(str, 0, str.size())));
  } else {
    for (size_t i = 0, i_max = str.size(); i != i_max; ++i) {
      auto chunk = translateAnyOfSubStr(str, i, i_max - i);
      if (chunk->translated()) {
        res.chunks().push_back(TranslationChunkPtr(chunk));
        //        i = chunk->orig_end();
      }
    }
  }
  return res;
}

dict::DeinflectTranslator::DeinflectTranslator(
    const fs::path &file, dict::Translator *next_translator)
    : DictionaryTranslator(), next_translator_(next_translator) {
  dicts_futures_.push_back(Loader::loadFromFS<DeinflectDictionary>(file));
}

dict::TranslationResult dict::DeinflectTranslator::doTranslate(
    const std::string &str, bool all) {
  return DictionaryTranslator::doTranslate(str, all);
}
