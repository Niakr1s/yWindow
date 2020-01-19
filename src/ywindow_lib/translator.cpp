#include "translator.h"

#include <algorithm>
#include <iostream>

#include "dictionary.h"
#include "json/json.h"
#include "loader.h"
#include "stringconvert.h"
#include "translationresult.h"

dict::Translator::~Translator() {}

dict::TranslationResult dict::Translator::translate(const std::wstring &wstr) {
  std::string str = WideStringToString(wstr);
  return translate(str);
}

dict::TranslationResult dict::Translator::translate(const std::string &str) {
  std::lock_guard<std::mutex> lock(mutex_);
  prepareDictionaries();
  auto res = doTranslate(str);
  //  res.normalize();
  return res;
}

void dict::Translator::reload() {
  prepareDictionaries();
  doReload();
}

void dict::Translator::setTranslatorsSettings(
    std::shared_ptr<dict::TranslatorsSettings> translators_settings) {
  std::lock_guard<std::mutex> lock(mutex_);
  doSetTranslatorsSettings(translators_settings);
}

void dict::Translator::prepareDictionaries() {}

dict::YomiTranslator::YomiTranslator(const fs::path &root_dir,
                                     Translator *deinflector)
    : DictionaryTranslator(deinflector), root_dir_(root_dir) {
  if (!fs::exists(root_dir)) {
    fs::create_directories(root_dir);
  }
  for (auto &dir : fs::directory_iterator(root_dir)) {
    if (!dir.is_directory()) {
      continue;
    }
    dicts_futures_[dir] = Loader::loadFromFS<YomiDictionary>(dir);
    paths_[dir] = fs::last_write_time(dir);
  }
}

void dict::YomiTranslator::doReload() {
  for (auto &dir : fs::directory_iterator(root_dir_)) {
    if (auto iter = paths_.find(dir); iter == paths_.end()) {
      dicts_futures_[dir] = Loader::loadFromFS<YomiDictionary>(dir);
      paths_[dir] = fs::last_write_time(dir);
    } else if (paths_[dir] != fs::last_write_time(dir)) {
      dicts_.erase(dir);
      dicts_futures_[dir] = Loader::loadFromFS<YomiDictionary>(dir);
      paths_[dir] = fs::last_write_time(dir);
    }
  }
}

dict::TranslationResult dict::YomiTranslator::doTranslate(
    const std::string &str) {
  return doTranslateAll<TranslatedChunkFinal>(str);
}

void dict::DictionaryTranslator::prepareDictionaries() {
  for (auto &[dir, dict] : dicts_futures_) {
    try {
      dicts_[dir] = std::unique_ptr<Dictionary>(dict.get());
    } catch (std::exception &e) {
      std::cout << "Couldn't load dictionary from " << dir
                << " , reason: " << e.what() << std::endl;
    }
  }
  dicts_futures_.clear();

  if (translators_settings_) {
    for (auto &[dir, dict] : dicts_) {
      if (translators_settings_->isNotIn(info(), *dict->info())) {
        translators_settings_->enableDictionary(info(), *dict->info());
      }
    }
    translators_settings_->saveJson();
  }
}

size_t dict::Translator::MAX_CHUNK_SIZE = 12;

dict::DictionaryTranslator::DictionaryTranslator(Translator *deinflector)
    : deinflector_(std::unique_ptr<Translator>(deinflector)) {}

void dict::DictionaryTranslator::setDeinflector(Translator *deinflector) {
  deinflector_ = std::unique_ptr<Translator>(deinflector);
}

void dict::DictionaryTranslator::doSetTranslatorsSettings(
    std::shared_ptr<dict::TranslatorsSettings> translators_settings) {
  translators_settings_ = translators_settings;
  prepareDictionaries();
}

dict::CardPtrs dict::DictionaryTranslator::queryAllNonDisabledDicts(
    const std::string &str) {
  CardPtrs res;
  for (auto &[dir, dict] : dicts_) {
    if (translators_settings_ &&
        translators_settings_->isDisabled(info(), *dict->info())) {
      continue;
    }

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
  TranslationResult transl_res{str};
  if (str.empty()) return transl_res;

  TranslationChunkPtrs translated_chunks;

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
    int res_second = it - it_beg;
    TranslationChunkPtr full_translated =
        doTranslateFullStr<TranslatedChunk_T>(inner_orig_text);
    if (full_translated->translated()) {
      return {full_translated, res_second};
    }
    if (deinflector_) {
      full_translated =
          doDeinflectAndTranslateFullStr<TranslatedChunk_T>(inner_orig_text);
      if (full_translated->translated()) {
        return {full_translated, res_second};
      }
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

  CardPtrs translations = queryAllNonDisabledDicts(str);
  if (translations.empty()) return std::make_shared<UntranslatedChunk>(str);

  CardPtrs sub_translations;
  for (auto it = transl_result.chunks().end() - 1,
            it_begin = transl_result.chunks().begin();
       it != it_begin; --it) {
    CardPtrs inner_sub_translations =
        queryAllNonDisabledDicts(TranslationResult{it_begin, it}.orig_text());
    sub_translations.insert(sub_translations.end(),
                            inner_sub_translations.cbegin(),
                            inner_sub_translations.cend());
  }
  return std::make_shared<TranslatedChunk_T>(str, std::move(translations),
                                             std::move(sub_translations));
}

template <class TranslatedChunk_T>
dict::TranslationChunkPtr
dict::DictionaryTranslator::doDeinflectAndTranslateFullStr(
    const std::string &str) {
  TranslationResult transl_result{str};
  TranslationChunkPtrs chunks_with_deinflection{transl_result.chunks().front()};

  for (auto it = transl_result.chunks().begin() + 1,
            it_end = transl_result.chunks().end();
       it != it_end; ++it) {
    std::string inner_str = TranslationResult{it, it_end}.orig_text();
    TranslationResult deinflected = deinflector_->translate(inner_str);

    if (deinflected.untranslatedSize() != 0 ||
        deinflected.chunks().size() != 1) {
      chunks_with_deinflection.push_back(*it);
      continue;
    }
    TranslationChunkPtr deinflected_chunk = deinflected.chunks().front();
    chunks_with_deinflection.push_back(deinflected_chunk);

    std::vector<TranslationText> texts =
        TranslationResult(chunks_with_deinflection.begin(),
                          chunks_with_deinflection.end())
            .toTexts();
    for (auto &text : texts) {
      TranslationChunkPtr ch =
          doTranslateFullStr<TranslatedChunk_T>(text.string());
      if (ch->translated()) {
        ch->setOriginText(str);
        return ch;
      }
    }
  }
  return std::make_shared<UntranslatedChunk>(str);
}

dict::DeinflectTranslator::DeinflectTranslator(const fs::path &file)
    : DictionaryTranslator() {
  if (fs::exists(file)) {
    dicts_futures_[file] = Loader::loadFromFS<DeinflectDictionary>(file);
  }
}

void dict::DeinflectTranslator::doReload() {}

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

void dict::ChainTranslator::doSetTranslatorsSettings(
    std::shared_ptr<dict::TranslatorsSettings> translators_settings) {
  for (auto &tr : translators_) {
    tr->setTranslatorsSettings(translators_settings);
  }
}

void dict::ChainTranslator::addTranslator(Translator *transl) {
  translators_.push_back(std::unique_ptr<Translator>(transl));
}

void dict::ChainTranslator::popTranslator() { translators_.pop_back(); }

void dict::ChainTranslator::doReload() {
  for (auto &t : translators_) {
    t->reload();
  }
}

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
    if (!buffer.empty()) {
      res = TranslationResult::join(buffer);
    }
  }
  return res;
}

dict::UserTranslator::UserTranslator(const fs::path &dir)
    : DictionaryTranslator(), dir_(dir) {
  if (fs::exists(dir)) {
    dicts_futures_[dir] = Loader::loadFromFS<UserDictionary>(dir);
    last_write_time_ = getDirLastWriteTime();
  }
}

void dict::UserTranslator::doReload() {}

void dict::UserTranslator::reloadFromFS() {
  if (fs::exists(dir_)) {
    fs::file_time_type changed_time = getDirLastWriteTime();
    if (changed_time != last_write_time_) {
      dicts_.clear();
      dicts_futures_[dir_] = Loader::loadFromFS<UserDictionary>(dir_);
      last_write_time_ = changed_time;
    }
  }
}

dict::TranslationResult dict::UserTranslator::doTranslate(
    const std::string &str) {
  return doTranslateAll<TranslatedUserChunk>(str);
}

void dict::UserTranslator::prepareDictionaries() {
  reloadFromFS();
  DictionaryTranslator::prepareDictionaries();
}

fs::file_time_type dict::UserTranslator::getDirLastWriteTime() {
  fs::file_time_type res = fs::last_write_time(dir_);
  for (auto &file : fs::directory_iterator(dir_)) {
    if (auto file_last_time = fs::last_write_time(file); file_last_time > res) {
      res = file_last_time;
    }
  }
  return res;
}
