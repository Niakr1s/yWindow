#include "translatorssettings.h"

#include <json/json.h>

#include <fstream>
#include <iostream>

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
    auto info = findDictionaryInfo(translator_info, dictionary_info);
    return info.enabled;
  } catch (std::out_of_range &) {
    return false;
  }
}

bool dict::TranslatorsSettings::isDisabled(const std::string &translator_info,
                                           const std::string &dictionary_info) {
  try {
    auto info = findDictionaryInfo(translator_info, dictionary_info);
    return !info.enabled;
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

void dict::TranslatorsSettings::addDictionary(
    const std::string &translator_info, const std::string &dictionary_info,
    const fs::path &path, bool enabled) {
  DictionaryInfo info;
  info.dictionary_info = dictionary_info;
  info.path = path;
  info.enabled = enabled;

  settings_[translator_info].insert(info);

  std::cout << "added dictionary: " << translator_info << ", "
            << dictionary_info << ", enabled: " << enabled << std::endl;
}

bool dict::TranslatorsSettings::isIn(const std::string &dictionary_info,
                                     const std::set<std::string> &vec) {
  if (std::find(vec.cbegin(), vec.cend(), dictionary_info) != vec.cend()) {
    return true;
  }
  return false;
}

dict::DictionaryInfo &dict::TranslatorsSettings::findDictionaryInfo(
    const std::string &translator_info, const std::string &dictionary_info) {
  auto &set = settings_.at(translator_info);
  for (auto &info : set) {
    if (info.dictionary_info == dictionary_info) {
      return const_cast<DictionaryInfo &>(info);
    }
  }
  throw(std::out_of_range("Not found dictionary info"));
}

void dict::TranslatorsSettings::enableDictionary(
    const std::string &translator_info, const std::string &dictionary_info) {
  try {
    auto &info = findDictionaryInfo(translator_info, dictionary_info);
    info.enabled = true;
    std::cout << "enabled dictionary: " << translator_info << ", "
              << dictionary_info;
  } catch (...) {
  }
}

void dict::TranslatorsSettings::disableDictionary(
    const std::string &translator_info, const std::string &dictionary_info) {
  try {
    auto &info = findDictionaryInfo(translator_info, dictionary_info);
    info.enabled = false;
    std::cout << "disabled dictionary: " << translator_info << ", "
              << dictionary_info << std::endl;
  } catch (...) {
  }
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

void dict::TranslatorsSettings::deleteDictionary(
    const std::string &translator_info, const std::string &dictionary_info) {
  try {
    auto &infos = settings_.at(translator_info);
    auto found = std::find(std::begin(infos), std::end(infos), dictionary_info);
    if (found != std::end(infos)) {
      settings_[translator_info].erase(found);
    }
  } catch (...) {
  }
}

void dict::TranslatorsSettings::updateDictionaryPath(
    const std::string &translator_info, const std::string &dictionary_info,
    const fs::path &path) {
  try {
    auto &dict_info = findDictionaryInfo(translator_info, dictionary_info);
    dict_info.path = path;
  } catch (...) {
  }
}

fs::path dict::TranslatorsSettings::getDictionaryPath(
    const std::string &translator_info, const std::string &dictionary_info) {
  auto dict = findDictionaryInfo(translator_info, dictionary_info);
  return dict.path;
}

void dict::TranslatorsSettings::deleteOtherDictionaries(
    const std::string &translator_info,
    const std::set<std::string> &dictionary_infos) {
  std::set<std::string> to_delete;

  auto infos = settings_[translator_info];
  for (auto &dict_info : infos) {
    if (dictionary_infos.find(dict_info.dictionary_info) ==
        dictionary_infos.end()) {  // if not in
      to_delete.insert(dict_info.dictionary_info);
    }
  }

  for (auto &dict_info : to_delete) {
    deleteDictionary(translator_info, dict_info);
  }
}

/*
[
    {
        "translator_info": "DeinflectTranslator",
        "dictionaries": [{
            "dictionary_info": "deinflect.json",
            "path": "data/blabla1",
            "enabled": true
        }, {
            "dictionary_info": "other.json",
            "path": "data/blabla2",
            "enabled": false
        }]
    }
]
*/

void dict::TranslatorsSettings::loadJson() {
  Json::Value root;
  std::ifstream is(json_file_);
  is >> root;
  if (root.empty()) return;
  for (int i = 0; i != root.size(); ++i) {
    std::string translator_info = root[i].get("translator_info", "").asString();
    if (translator_info.empty()) continue;

    Json::Value &dictionaries = root[i]["dictionaries"];

    for (int j = 0; j != dictionaries.size(); ++j) {
      std::string dictionary_info =
          dictionaries[j].get("dictionary_info", "").asString();
      fs::path path = dictionaries[j].get("path", "").asString();
      bool enabled = dictionaries[j].get("enabled", true).asBool();

      DictionaryInfo dict_info;
      dict_info.dictionary_info = dictionary_info;
      dict_info.path = path;
      dict_info.enabled = enabled;

      settings_[translator_info].insert(dict_info);
    }
  }
}

void dict::TranslatorsSettings::saveJson() {
  Json::Value root;
  for (auto &[translator_info, dict_infos] : settings_) {
    Json::Value translator;
    translator["translator_info"] = translator_info;

    Json::Value &dicts = translator["dictionaries"];

    for (auto &dict_info : dict_infos) {
      Json::Value dict;
      dict["dictionary_info"] = dict_info.dictionary_info;
      dict["path"] = dict_info.path.string();
      dict["enabled"] = dict_info.enabled;
      dicts.append(dict);
    }
    root.append(translator);
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
    res += dict_settings.size();
  }
  return res;
}

const std::map<std::string, std::set<dict::DictionaryInfo>>
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
