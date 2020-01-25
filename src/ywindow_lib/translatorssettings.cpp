#include "translatorssettings.h"

#include <json/json.h>

#include <fstream>

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

void dict::TranslatorsSettings::deleteDictionary(
    const std::string &translator_info, const std::string &dictionary_info) {
  settings_[translator_info].enabled.erase(dictionary_info);
  settings_[translator_info].disabled.erase(dictionary_info);
}

void dict::TranslatorsSettings::deleteOtherDictionaries(
    const std::string &translator_info,
    const std::set<std::string> &dictionary_infos) {
  std::set<std::string> to_delete;

  auto fill = [&](const std::set<std::string> &from) {
    for (auto &dict_info : from) {
      if (dictionary_infos.find(dict_info) ==
          dictionary_infos.end()) {  // if not in
        to_delete.insert(dict_info);
      }
    }
  };

  fill(settings_[translator_info].enabled);
  fill(settings_[translator_info].disabled);

  for (auto &dict_info : to_delete) {
    deleteDictionary(translator_info, dict_info);
  }
}

void dict::TranslatorsSettings::loadJson() {
  Json::Value root;
  std::ifstream is(json_file_);
  is >> root;
  if (root.empty()) return;
  for (int i = 0; i != root.size(); ++i) {
    DictionarySettings state;
    std::string translator_info = root[i].get("translator_info", "").asString();
    if (translator_info.empty()) continue;

    Json::Value &enabled = root[i][ENABLED];
    Json::Value &disabled = root[i][DISABLED];

    if (!enabled.empty()) {
      for (int j = 0; j != enabled.size(); ++j) {
        state.enabled.insert(enabled[j].asString());
      }
    }
    if (!disabled.empty()) {
      for (int j = 0; j != disabled.size(); ++j) {
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
