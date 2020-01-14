#include "translator.h"

#include <algorithm>

#include "dictionary.h"
#include "json/json.h"
#include "loader.h"
#include "stringconvert.h"
#include "translationresult.h"

dict::TranslatorsSettings::TranslatorsSettings(
    const std::filesystem::path &json_file)
    : json_file_(json_file) {
  if (!fs::exists(json_file_)) {
    fs::create_directories(json_file_.parent_path());
    std::ofstream f(json_file_);
  } else {
    loadJson();
  }
  last_write_time_ = fs::last_write_time(json_file_);
}

void dict::TranslatorsSettings::updateFromFS() {
  if (fileChanged()) {
    loadJson();
  }
}

bool dict::TranslatorsSettings::isInOrdered(
    const std::string &translator_info, const std::string &dictionary_info) {
  try {
    auto &state = settings_.at(translator_info);
    return isIn(dictionary_info, state.ordered);
  } catch (std::out_of_range &) {
    return false;
  }
}

bool dict::TranslatorsSettings::isInUnordered(
    const std::string &translator_info, const std::string &dictionary_info) {
  try {
    auto &state = settings_.at(translator_info);
    return isIn(dictionary_info, state.unordered);
  } catch (std::out_of_range &) {
    return false;
  }
}

bool dict::TranslatorsSettings::isInDisabled(
    const std::string &translator_info, const std::string &dictionary_info) {
  try {
    auto &state = settings_.at(translator_info);
    return isIn(dictionary_info, state.disabled);
  } catch (std::out_of_range &) {
    return false;
  }
}

bool dict::TranslatorsSettings::isIn(const std::string &translator_info,
                                     const std::string &dictionary_info) {
  return isInOrdered(translator_info, dictionary_info) ||
         isInUnordered(translator_info, dictionary_info) ||
         isInDisabled(translator_info, dictionary_info);
}

bool dict::TranslatorsSettings::notIn(const std::string &translator_info,
                                      const std::string &dictionary_info) {
  return !isInOrdered(translator_info, dictionary_info) &&
         !isInUnordered(translator_info, dictionary_info) &&
         !isInDisabled(translator_info, dictionary_info);
}

bool dict::TranslatorsSettings::isIn(const std::string &dictionary_info,
                                     const std::set<std::string> &vec) {
  if (std::find(vec.cbegin(), vec.cend(), dictionary_info) != vec.cend()) {
    return true;
  }
  return false;
}

void dict::TranslatorsSettings::addUnorderedDictionary(
    const std::string &translator_info, const std::string &dictionary_info) {
  settings_[translator_info].unordered.insert(dictionary_info);
}

void dict::TranslatorsSettings::loadJson() {
  Json::Value root;
  std::ifstream is(json_file_);
  is >> root;
  if (root.empty()) return;
  for (size_t i = 0; i != root.size(); ++i) {
    DictionarySettings state;
    std::string translator_info = root[i].get("translator_info", "").asString();
    if (translator_info.empty()) continue;

    Json::Value &ordered = root[i][ORDERED];
    Json::Value &unordered = root[i][UNORDERED];
    Json::Value &disabled = root[i][DISABLED];

    if (!ordered.empty()) {
      for (size_t j = 0; j != ordered.size(); ++j) {
        state.ordered.insert(ordered[j].asString());
      }
    }
    if (!unordered.empty()) {
      for (size_t j = 0; j != unordered.size(); ++j) {
        state.unordered.insert(unordered[j].asString());
      }
    }
    if (!disabled.empty()) {
      for (size_t j = 0; j != disabled.size(); ++j) {
        state.disabled.insert(disabled[j].asString());
      }
    }
    settings_[translator_info] = state;
  }
}

void dict::TranslatorsSettings::saveJson() {
  Json::Value root;
  for (auto &[translator_info, state] : settings_) {
    Json::Value item;
    item["translator_info"] = translator_info;
    Json::Value ordered, unordered, disabled;
    for (auto &it : state.ordered) {
      ordered.append(it);
    }
    for (auto &it : state.unordered) {
      unordered.append(it);
    }
    for (auto &it : state.disabled) {
      disabled.append(it);
    }
    if (!ordered.empty()) {
      item[ORDERED] = ordered;
    }
    if (!unordered.empty()) {
      item[UNORDERED] = unordered;
    }
    if (!disabled.empty()) {
      item[DISABLED] = disabled;
    }
    root.append(item);
  }
  {
    std::ofstream os(json_file_);
    os << root;
  }
  last_write_time_ = fs::last_write_time(json_file_);
}

bool dict::TranslatorsSettings::fileChanged() {
  fs::file_time_type current_write_time = fs::last_write_time(json_file_);
  bool changed = current_write_time != last_write_time_;
  if (changed) {
    last_write_time_ = current_write_time;
  }
  return changed;
}

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

dict::YomiTranslator::YomiTranslator(const fs::path &root_dir,
                                     Translator *deinflector)
    : DictionaryTranslator(deinflector) {
  if (!fs::exists(root_dir)) {
    fs::create_directories(root_dir);
  }
  for (auto &path : fs::directory_iterator(root_dir)) {
    if (!path.is_directory()) {
      continue;
    }
    dicts_futures_.push_back(Loader::loadFromFS<YomiDictionary>(path));
  }
}

dict::YomiTranslator::YomiTranslator(std::initializer_list<fs::path> dicts_dirs,
                                     Translator *deinflector)
    : DictionaryTranslator(deinflector) {
  for (auto &dir : dicts_dirs) {
    if (fs::exists(dir)) {
      dicts_futures_.push_back(Loader::loadFromFS<YomiDictionary>(dir));
    }
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
  if (translators_settings_) {
    translators_settings_->updateFromFS();
    bool json_need_save = false;
    for (auto &dict : dicts_) {
      if (translators_settings_->notIn(info(), dict->info())) {
        translators_settings_->addUnorderedDictionary(info(), dict->info());
        json_need_save = true;
      }
    }
    if (json_need_save) {
      translators_settings_->saveJson();
    }
  }
}

size_t dict::Translator::MAX_CHUNK_SIZE = 12;

dict::DictionaryTranslator::DictionaryTranslator(Translator *deinflector)
    : deinflector_(std::unique_ptr<Translator>(deinflector)) {}

void dict::DictionaryTranslator::addDict(dict::Dictionary *dict) {
  dicts_.push_back(std::unique_ptr<Dictionary>(dict));
}

void dict::DictionaryTranslator::setDeinflector(Translator *deinflector) {
  deinflector_ = std::unique_ptr<Translator>(deinflector);
}

void dict::DictionaryTranslator::setTranslatorsSettings(
    std::shared_ptr<dict::TranslatorsSettings> translators_settings) {
  translators_settings_ = translators_settings;
  prepareDictionaries();
}

dict::CardPtrs dict::DictionaryTranslator::queryAllNonDisabledDicts(
    const std::string &str) {
  CardPtrs res;
  for (auto &dict : dicts_) {
    if (translators_settings_ &&
        translators_settings_->isInDisabled(info(), dict->info())) {
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
    dicts_futures_.push_back(Loader::loadFromFS<DeinflectDictionary>(file));
  }
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

void dict::ChainTranslator::setTranslatorsSettings(
    std::shared_ptr<dict::TranslatorsSettings> translators_settings) {
  for (auto &tr : translators_) {
    tr->setTranslatorsSettings(translators_settings);
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
    if (!buffer.empty()) {
      res = TranslationResult::join(buffer);
    }
  }
  return res;
}

dict::UserTranslator::UserTranslator(const fs::path &file)
    : DictionaryTranslator() {
  if (fs::exists(file)) {
    dicts_futures_.push_back(Loader::loadFromFS<UserDictionary>(file));
  }
}

dict::TranslationResult dict::UserTranslator::doTranslate(
    const std::string &str) {
  return doTranslateAll<TranslatedUserChunk>(str);
}
