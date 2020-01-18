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

dict::TranslatorsSettings::~TranslatorsSettings() { saveJson(); }

bool dict::TranslatorsSettings::isEnabled(const std::string &translator_info,
                                          const std::string &dictionary_info) {
  try {
    auto &state = settings_.at(translator_info);
    return isIn(dictionary_info, state.enabled);
  } catch (std::out_of_range &) {
    return false;
  }
}

bool dict::TranslatorsSettings::isDisabled(const std::string &translator_info,
                                           const std::string &dictionary_info) {
  try {
    auto &state = settings_.at(translator_info);
    return isIn(dictionary_info, state.disabled);
  } catch (std::out_of_range &) {
    return false;
  }
}

bool dict::TranslatorsSettings::isIn(const std::string &translator_info,
                                     const std::string &dictionary_info) {
  return isEnabled(translator_info, dictionary_info) ||
         isDisabled(translator_info, dictionary_info);
}

bool dict::TranslatorsSettings::isNotIn(const std::string &translator_info,
                                        const std::string &dictionary_info) {
  return !isEnabled(translator_info, dictionary_info) &&
         !isDisabled(translator_info, dictionary_info);
}

bool dict::TranslatorsSettings::isIn(const std::string &dictionary_info,
                                     const std::set<std::string> &vec) {
  if (std::find(vec.cbegin(), vec.cend(), dictionary_info) != vec.cend()) {
    return true;
  }
  return false;
}

void dict::TranslatorsSettings::enableDictionary(
    const std::string &translator_info, const std::string &dictionary_info) {
  settings_[translator_info].disabled.erase(dictionary_info);
  settings_[translator_info].enabled.insert(dictionary_info);
}

void dict::TranslatorsSettings::disableDictionary(
    const std::string &translator_info, const std::string &dictionary_info) {
  settings_[translator_info].enabled.erase(dictionary_info);
  settings_[translator_info].disabled.insert(dictionary_info);
}

void dict::TranslatorsSettings::moveDictionary(
    const std::string &translator_info, const std::string &dictionary_info,
    bool enabled) {
  if (enabled) {
    enableDictionary(translator_info, dictionary_info);
  } else {
    disableDictionary(translator_info, dictionary_info);
  }
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

    Json::Value &enabled = root[i][ENABLED];
    Json::Value &disabled = root[i][DISABLED];

    if (!enabled.empty()) {
      for (size_t j = 0; j != enabled.size(); ++j) {
        state.enabled.insert(enabled[j].asString());
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
    Json::Value enabled, disabled;

    for (auto &it : state.enabled) {
      enabled.append(it);
    }
    for (auto &it : state.disabled) {
      disabled.append(it);
    }
    if (!enabled.empty()) {
      item[ENABLED] = enabled;
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

int dict::TranslatorsSettings::size() const {
  int res = 0;
  for (auto &[_, dict_settings] : settings_) {
    res += dict_settings.enabled.size() + dict_settings.disabled.size();
  }
  return res;
}

const std::map<std::string, dict::DictionarySettings>
    &dict::TranslatorsSettings::settings() const {
  return settings_;
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
  std::lock_guard<std::mutex> lock(mutex_);
  prepareDictionaries();
  auto res = doTranslate(str);
  //  res.normalize();
  return res;
}

void dict::Translator::setTranslatorsSettings(
    std::shared_ptr<dict::TranslatorsSettings> translators_settings) {
  std::lock_guard<std::mutex> lock(mutex_);
  doSetTranslatorsSettings(translators_settings);
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
    for (auto &dict : dicts_) {
      if (translators_settings_->isNotIn(info(), dict->info())) {
        translators_settings_->enableDictionary(info(), dict->info());
      }
    }
    translators_settings_->saveJson();
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

void dict::DictionaryTranslator::doSetTranslatorsSettings(
    std::shared_ptr<dict::TranslatorsSettings> translators_settings) {
  translators_settings_ = translators_settings;
  prepareDictionaries();
}

dict::CardPtrs dict::DictionaryTranslator::queryAllNonDisabledDicts(
    const std::string &str) {
  CardPtrs res;
  for (auto &dict : dicts_) {
    if (translators_settings_ &&
        translators_settings_->isDisabled(info(), dict->info())) {
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
    : DictionaryTranslator() {
  if (fs::exists(dir)) {
    dicts_futures_.push_back(Loader::loadFromFS<UserDictionary>(dir));
  }
}

dict::TranslationResult dict::UserTranslator::doTranslate(
    const std::string &str) {
  return doTranslateAll<TranslatedUserChunk>(str);
}
