#include "translator.h"

#include <algorithm>

#include "dictionary.h"
#include "loader.h"
#include "stringconvert.h"

dict::Translator::~Translator() {}

dict::TranslationResult dict::Translator::translate(const std::wstring &wstr) {
  std::string str = WideStringToString(wstr);
  return translate(str);
}

dict::TranslationResult dict::Translator::translate(const std::string &str) {
  prepareDictionaries();
  auto res = doTranslate(str);
  //  res.normalize();
  return res;
}

void dict::Translator::prepareDictionaries() {}

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

dict::TranslationResult dict::YomiTranslator::doTranslate(
    const std::string &str) {
  return doTranslateAll<TranslatedChunkFinal>(str);
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

size_t dict::Translator::MAX_CHUNK_SIZE = 12;

dict::DictionaryTranslator::DictionaryTranslator() {}

void dict::DictionaryTranslator::addDict(dict::Dictionary *dict) {
  dicts_.push_back(std::unique_ptr<Dictionary>(dict));
}

dict::CardPtrs dict::DictionaryTranslator::queryAllDicts(
    const std::string &str) {
  CardPtrs res;
  for (auto &dict : dicts_) {
    auto query = dict->query(str);
    for (auto &card : query) {
      res.push_back(card);
    }
  }
  return res;
}

template <class TranslatedChunk_T>
dict::TranslationResult dict::DictionaryTranslator::doTranslateAll(
    const std::string &str) {
  TranslationChunkPtrs translated_chunks;

  TranslationResult transl_res{str};
  for (auto it = transl_res.chunks().begin(),
            it_end = transl_res.chunks().end();
       it < it_end; ++it) {
    TranslationResult inner{it, it_end};
    auto chunk = doTranslateFirstOf<TranslatedChunk_T>(inner.orig_text());
    if (chunk.first->translated()) {
      translated_chunks.push_back(chunk.first);
      it += chunk.second - 1;
    } else {
      translated_chunks.push_back(*it);
    }
  }
  return TranslationResult{translated_chunks.begin(), translated_chunks.end()};
}

template <class TranslatedChunk_T>
std::pair<dict::TranslationChunkPtr, int>
dict::DictionaryTranslator::doTranslateFirstOf(const std::string &str) {
  TranslationResult transl_result{str};

  auto diff = std::min(transl_result.chunks().size(), MAX_CHUNK_SIZE);
  for (auto it = transl_result.chunks().begin() + diff,
            it_beg = transl_result.chunks().begin();
       it != it_beg; --it) {
    auto inner_orig_text = TranslationResult{it_beg, it}.orig_text();
    CardPtrs translations = queryAllDicts(inner_orig_text);

    if (!translations.empty()) {
      CardPtrs sub_translations;
      for (auto inner_it = it - 1; inner_it != it_beg; --inner_it) {
        CardPtrs inner_sub_translations =
            queryAllDicts(TranslationResult{it_beg, inner_it}.orig_text());
        sub_translations.insert(sub_translations.end(),
                                inner_sub_translations.cbegin(),
                                inner_sub_translations.cend());
      }
      return {std::make_shared<TranslatedChunk_T>(inner_orig_text,
                                                  std::move(translations),
                                                  std::move(sub_translations)),
              it - it_beg};
    }
  }
  return {std::make_shared<UntranslatedChunk>(str), 0};
}

template <class TranslatedChunk_T>
dict::TranslationChunkPtr dict::DictionaryTranslator::doTranslateFullStr(
    const std::string &str) {
  TranslationResult transl_result{str};
  if (transl_result.size() == 0)
    return std::make_shared<UntranslatedChunk>(str);

  CardPtrs translations = queryAllDicts(str);
  if (translations.empty()) return std::make_shared<UntranslatedChunk>(str);

  CardPtrs sub_translations;
  for (auto it = transl_result.chunks().end() - 1,
            it_begin = transl_result.chunks().begin();
       it != it_begin; --it) {
    CardPtrs inner_sub_translations =
        queryAllDicts(TranslationResult{it_begin, it}.orig_text());
    sub_translations.insert(sub_translations.end(),
                            inner_sub_translations.cbegin(),
                            inner_sub_translations.cend());
  }
  return std::make_shared<TranslatedChunk_T>(str, std::move(translations),
                                             std::move(sub_translations));
}

dict::DeinflectTranslator::DeinflectTranslator(
    const fs::path &file, dict::Translator *next_translator)
    : DictionaryTranslator(), next_translator_(next_translator) {
  dicts_futures_.push_back(Loader::loadFromFS<DeinflectDictionary>(file));
}

dict::TranslationResult dict::DeinflectTranslator::doTranslate(
    const std::string &str) {
  return doTranslateAll<TranslatedChunk>(str);
}

dict::ChainTranslator::ChainTranslator() {}

dict::ChainTranslator::ChainTranslator(
    std::initializer_list<dict::Translator *> translators) {
  for (auto &tr : translators) {
    translators_.push_back(std::unique_ptr<Translator>(tr));
  }
}

void dict::ChainTranslator::addTranslator(Translator *transl) {
  translators_.push_back(std::unique_ptr<Translator>(transl));
}

void dict::ChainTranslator::popTranslator() { translators_.pop_back(); }

dict::TranslationResult dict::ChainTranslator::doTranslate(
    const std::string &str) {
  if (translators_.empty()) return TranslationResult(str);
  auto translator_it = translators_.begin(),
       translator_it_end = translators_.end();
  TranslationResult res = (*translator_it)->translate(str);
  while (++translator_it != translator_it_end) {
    std::vector<TranslationResult> splitted = res.splitByFinal();
    std::vector<TranslationResult> buffer;
    for (auto &ch : splitted) {
      if (ch.final()) {
        buffer.push_back(ch);
      } else {
        auto texts = ch.toTexts();
        std::vector<TranslationResult> merged;
        for (auto &text : texts) {
          auto ch_translated = (*translator_it)->translate(text.string());

          std::vector<TranslationResult> merged_inner =
              ch.mergeWithNextTranslation(ch_translated);
          merged.insert(merged.end(), merged_inner.begin(), merged_inner.end());
        }
        //        auto ch_translated =
        //        (*translator_it)->translate(ch.orig_text()); auto merged =
        //        ch.mergeWithNextTranslation(ch_translated);
        buffer.push_back(TranslationResult::bestTranslation(merged));
      }
    }
    res = TranslationResult::join(buffer);
  }
  return res;
}

dict::UserTranslator::UserTranslator(const fs::path &file)
    : DictionaryTranslator() {
  dicts_futures_.push_back(Loader::loadFromFS<UserDictionary>(file));
}

dict::TranslationResult dict::UserTranslator::doTranslate(
    const std::string &str) {
  return doTranslateAll<TranslatedChunkFinal>(str);
}
