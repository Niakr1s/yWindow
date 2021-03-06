#include "translator.h"

#include <algorithm>
#include <iostream>

#include "dictionary.h"
#include "json/json.h"
#include "loader.h"
#include "stringconvert.h"
#include "translationresult.h"

// Translator

dict::Translator::~Translator() {}

dict::TranslationResult dict::Translator::translate(const std::wstring &wstr) {
  std::string str = WideStringToString(wstr);
  return translate(str);
}

dict::TranslationResult dict::Translator::translate(const std::string &str) {
  std::lock_guard<std::mutex> lock(mutex_);
  doPrepareDictionaries();
  doReload();
  doPrepareDictionaries();
  doUpdateTranslatorsSettings();
  return doTranslate(str);
}

void dict::Translator::reload() {
  std::lock_guard<std::mutex> lock(mutex_);
  doPrepareDictionaries();
  doReload();
  doPrepareDictionaries();
  doUpdateTranslatorsSettings();
}

void dict::Translator::setTranslatorsSettings(
    std::shared_ptr<dict::TranslatorsSettings> translators_settings) {
  std::lock_guard<std::mutex> lock(mutex_);
  doSetTranslatorsSettings(translators_settings);
  doPrepareDictionaries();
  doUpdateTranslatorsSettings();
}

size_t dict::Translator::MAX_CHUNK_SIZE = 12;

// DirectoryTranslator

template <class Dict>
dict::DirectoryTranslator<Dict>::DirectoryTranslator(
    const std::filesystem::path &root_dir, Translator *deinflector)
    : deinflector_(std::unique_ptr<Translator>(deinflector)),
      root_dir_(root_dir) {
  if (!fs::exists(root_dir)) {
    fs::create_directories(root_dir);
  }
  for (auto &dir : fs::directory_iterator(root_dir)) {
    addFutureAndUpdateLastWriteTime(dir);
  }
}

template <class Dict>
void dict::DirectoryTranslator<Dict>::doPrepareDictionaries() {
  futuresToDicts();
  makeProxyCards();
}

template <class Dict>
void dict::DirectoryTranslator<Dict>::setDeinflector(Translator *deinflector) {
  deinflector_ = std::unique_ptr<Translator>(deinflector);
}

template <class Dict>
void dict::DirectoryTranslator<Dict>::doSetTranslatorsSettings(
    std::shared_ptr<dict::TranslatorsSettings> translators_settings) {
  translators_settings_ = translators_settings;
}

template <class Dict>
void dict::DirectoryTranslator<Dict>::doUpdateTranslatorsSettings() {
  if (!translators_settings_) return;

  for (auto &[dir, dict] : dicts_) {
    if (translators_settings_->isNotIn(info(), *dict->info())) {
      translators_settings_->addDictionary(info(), *dict->info(), dir);
    } else {
      translators_settings_->updateDictionaryPath(info(), *dict->info(), dir);
    }
  }

  std::set<std::string> infos;
  for (auto &[dir, dict] : dicts_) {
    infos.insert(*dict->info());
  }
  translators_settings_->deleteOtherDictionaries(info(), infos);

  translators_settings_->saveJson();
}

template <class Dict>
void dict::DirectoryTranslator<Dict>::doReload() {
  std::set<fs::path> dirs_in_root_dir, to_delete;

  for (auto &dir : fs::directory_iterator(root_dir_)) {
    dirs_in_root_dir.insert(dir);

    if (auto iter = dicts_.find(dir); iter == dicts_.end()) {
      addFutureAndUpdateLastWriteTime(dir);
    } else if (paths_[dir] != fs::last_write_time(dir)) {
      dicts_.erase(dir);
      addFutureAndUpdateLastWriteTime(dir);
    }
  }

  for (auto &[dir, _] : dicts_) {
    if (dirs_in_root_dir.find(dir) == dirs_in_root_dir.end()) {
      to_delete.insert(dir);
    }
  }

  for (auto &dir : to_delete) {
    dicts_.erase(dir);
  }
}

template <class Dict>
void dict::DirectoryTranslator<Dict>::futuresToDicts() {
  for (auto &[dir, dict] : dicts_futures_) {
    try {
      dicts_[dir] = std::unique_ptr<Dictionary>(dict.get());
    } catch (std::exception &e) {
      std::cout << "Couldn't load dictionary from " << dir
                << " , reason: " << e.what() << std::endl;
    }
  }
  dicts_futures_.clear();
}

template <class Dict>
void dict::DirectoryTranslator<Dict>::makeProxyCards() {
  for (auto &[_, dict] : dicts_) {
    dict->makeProxyCards();
  }
}

template <class Dict>
dict::CardPtrs dict::DirectoryTranslator<Dict>::queryAllNonDisabledDicts(
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

template <class Dict>
template <class TranslatedChunk_T>
dict::TranslationResult dict::DirectoryTranslator<Dict>::doTranslateAll(
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

template <class Dict>
template <class TranslatedChunk_T>
std::pair<dict::TranslationChunkPtr, int>
dict::DirectoryTranslator<Dict>::doTranslateFirstOf(const std::string &str) {
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

template <class Dict>
template <class TranslatedChunk_T>
dict::TranslationChunkPtr dict::DirectoryTranslator<Dict>::doTranslateFullStr(
    const std::string &str) {
  TranslationResult transl_result{str};
  if (transl_result.size() == 0)
    return std::make_shared<UntranslatedChunk>(str);

  CardPtrs translations = queryAllNonDisabledDicts(str);
  if (translations.empty()) return std::make_shared<UntranslatedChunk>(str);

  CardPtrs sub_translations;
  for (auto it_begin = transl_result.chunks().begin();
       it_begin != transl_result.chunks().end(); ++it_begin) {
    for (auto it = transl_result.chunks().end(); it != it_begin; --it) {
      // skipping full text, because it is already in translations
      if (it_begin == transl_result.chunks().begin() &&
          it == transl_result.chunks().end()) {
        continue;
      }

      CardPtrs inner_sub_translations =
          queryAllNonDisabledDicts(TranslationResult{it_begin, it}.orig_text());

      CardPtrs inner_sub_translations_filtered;
      std::copy_if(std::cbegin(inner_sub_translations),
                   std::cend(inner_sub_translations),
                   std::back_inserter(inner_sub_translations_filtered),
                   [](const CardPtr &card) {
                     return !card->isProxy() && card->isKanji();
                   });

      sub_translations.insert(sub_translations.end(),
                              inner_sub_translations_filtered.cbegin(),
                              inner_sub_translations_filtered.cend());
    }
  }
  return std::make_shared<TranslatedChunk_T>(str, std::move(translations),
                                             std::move(sub_translations));
}

template <class Dict>
template <class TranslatedChunk_T>
dict::TranslationChunkPtr
dict::DirectoryTranslator<Dict>::doDeinflectAndTranslateFullStr(
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

template <class Dict>
void dict::DirectoryTranslator<Dict>::addFutureAndUpdateLastWriteTime(
    const std::filesystem::path &path) {
  dicts_futures_[path] = Loader::loadFromFS<Dict>(path);
  paths_[path] = fs::last_write_time(path);
}

// YomiTranslator

dict::YomiTranslator::YomiTranslator(const fs::path &root_dir,
                                     Translator *deinflector)
    : DirectoryTranslator(root_dir, deinflector) {}

dict::TranslationResult dict::YomiTranslator::doTranslate(
    const std::string &str) {
  return doTranslateAll<TranslatedChunkFinal>(str);
}

// DeinflectTranslator

dict::DeinflectTranslator::DeinflectTranslator(const fs::path &root_dir)
    : DirectoryTranslator(root_dir) {}

dict::TranslationResult dict::DeinflectTranslator::doTranslate(
    const std::string &str) {
  return doTranslateAll<TranslatedChunk>(str);
}

// UserTranslator

dict::UserTranslator::UserTranslator(const fs::path &root_dir)
    : DirectoryTranslator(root_dir) {}

dict::TranslationResult dict::UserTranslator::doTranslate(
    const std::string &str) {
  return doTranslateAll<TranslatedUserChunk>(str);
}

// ChainTranslator

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
    for (auto &transl_res : splitted) {
      if (transl_res.user()) {
        for (auto &ch : transl_res.chunks()) {
          auto translated = (*translator_it)->translate(ch->originText());
          if (translated.size() == 1) {
            ch->addTranslations(*translated.chunks().front());
          }
          for (auto &translated_ch : translated.chunks()) {
            for (auto &transl : translated_ch->translations()) {
              if (transl->isKanji()) ch->addSubTranslation(transl);
            }
            for (auto &transl : translated_ch->subTranslations()) {
              if (transl->isKanji()) ch->addSubTranslation(transl);
            }
          }
        }
        buffer.push_back(transl_res);
      } else if (transl_res.final()) {
        buffer.push_back(transl_res);
      } else {
        auto texts = transl_res.toTexts();
        std::vector<TranslationResult> merged;
        for (auto &text : texts) {
          auto ch_translated = (*translator_it)->translate(text.string());

          std::vector<TranslationResult> merged_inner =
              transl_res.mergeWithNextTranslation(ch_translated);
          merged.insert(merged.end(), merged_inner.begin(), merged_inner.end());
        }
        buffer.push_back(TranslationResult::bestTranslation(merged));
      }
    }
    if (!buffer.empty()) {
      res = TranslationResult::join(buffer);
    }
  }
  return res;
}

void dict::ChainTranslator::doPrepareDictionaries() {}

void dict::ChainTranslator::doUpdateTranslatorsSettings() {
  for (auto &t : translators_) {
    t->doUpdateTranslatorsSettings();
  }
}

const std::string dict::YOMI_TRANSLATOR_INFO = "YomiTranslator";
const std::string dict::DEINFLECT_TRANSLATOR_INFO = "DeinflectTranslator";
const std::string dict::USER_TRANSLATOR_INFO = "UserTranslator";
const std::string dict::CHAIN_TRANSLATOR_INFO = "ChainTranslator";
