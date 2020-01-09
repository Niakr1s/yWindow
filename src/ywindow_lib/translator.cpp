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

size_t dict::Translator::MAX_CHUNK_SIZE = 12;

dict::DictionaryTranslator::DictionaryTranslator() {}

void dict::DictionaryTranslator::addDict(dict::Dictionary *dict) {
  dicts_.push_back(std::unique_ptr<Dictionary>(dict));
}

dict::CardPtrMultiMap dict::DictionaryTranslator::queryAllDicts(
    const std::string &str) {
  CardPtrMultiMap res;
  for (auto &dict : dicts_) {
    auto query = dict->query(str);
    for (auto &card : query) {
      res.insert(card);
    }
  }
  return res;
}

dict::TranslationResult dict::DictionaryTranslator::doTranslate(
    const std::string &str, bool all) {
  return doTranslateAll(str);
}

dict::TranslationResult dict::DictionaryTranslator::doTranslateAll(
    const std::string &str) {
  TranslationChunkPtrs translated_chunks;

  TranslationResult transl_res{str};
  for (auto it = transl_res.chunks().begin(),
            it_end = transl_res.chunks().end();
       it < it_end; ++it) {
    TranslationResult inner{it, it_end};
    auto chunk = doTranslateFirstOf(inner.orig_text());
    if (chunk.first->translated()) {
      translated_chunks.push_back(chunk.first);
      it += chunk.second - 1;
    } else {
      translated_chunks.push_back(*it);
    }
  }
  return TranslationResult{translated_chunks.begin(), translated_chunks.end()};
}

std::pair<dict::TranslationChunkPtr, int>
dict::DictionaryTranslator::doTranslateFirstOf(const std::string &str) {
  TranslationResult transl_result{str};

  auto diff = std::min(transl_result.chunks().size(), MAX_CHUNK_SIZE);
  for (auto it = transl_result.chunks().begin() + diff,
            it_beg = transl_result.chunks().begin();
       it != it_beg; --it) {
    auto inner_orig_text = TranslationResult{it_beg, it}.orig_text();
    CardPtrMultiMap translations = queryAllDicts(inner_orig_text);

    if (!translations.empty()) {
      CardPtrMultiMap sub_translations;
      for (auto inner_it = it - 1; inner_it != it_beg; --inner_it) {
        CardPtrMultiMap inner_sub_translations =
            queryAllDicts(TranslationResult{it_beg, inner_it}.orig_text());
        sub_translations.insert(inner_sub_translations.cbegin(),
                                inner_sub_translations.cend());
      }
      return {std::make_shared<TranslatedChunk>(inner_orig_text,
                                                std::move(translations),
                                                std::move(sub_translations)),
              it - it_beg};
    }
  }
  return {std::make_shared<UntranslatedChunk>(str), 0};
}

dict::TranslationChunkPtr dict::DictionaryTranslator::doTranslateFullStr(
    const std::string &str) {
  TranslationResult transl_result{str};
  if (transl_result.size() == 0)
    return std::make_shared<UntranslatedChunk>(str);

  CardPtrMultiMap translations = queryAllDicts(str);
  if (translations.empty()) return std::make_shared<UntranslatedChunk>(str);

  CardPtrMultiMap sub_translations;
  for (auto it = transl_result.chunks().end() - 1,
            it_begin = transl_result.chunks().begin();
       it != it_begin; --it) {
    CardPtrMultiMap inner_sub_translations =
        queryAllDicts(TranslationResult{it_begin, it}.orig_text());
    sub_translations.insert(inner_sub_translations.cbegin(),
                            inner_sub_translations.cend());
  }
  return std::make_shared<TranslatedChunk>(str, std::move(translations),
                                           std::move(sub_translations));
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
