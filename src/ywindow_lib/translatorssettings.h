#ifndef TRANSLATORSSETTINGS_H
#define TRANSLATORSSETTINGS_H

#include <filesystem>
#include <map>
#include <set>

namespace fs = std::filesystem;

namespace dict {

class Translator;

struct DictionarySettings {
  std::set<std::string> enabled, disabled;
};

class TranslatorsSettings {
 public:
  TranslatorsSettings(const fs::path& json_file);
  ~TranslatorsSettings();

  bool isEnabled(const std::string& translator_info,
                 const std::string& dictionary_info);
  bool isDisabled(const std::string& translator_info,
                  const std::string& dictionary_info);
  bool isIn(const std::string& translator_info,
            const std::string& dictionary_info);
  bool isNotIn(const std::string& translator_info,
               const std::string& dictionary_info);

  // don't forget to saveJson in the end
  void enableDictionary(const std::string& translator_info,
                        const std::string& dictionary_info);
  void disableDictionary(const std::string& translator_info,
                         const std::string& dictionary_info);
  void moveDictionary(const std::string& translator_info,
                      const std::string& dictionary_info, bool enabled);
  void saveJson();

  int size() const;

  const std::map<std::string, DictionarySettings>& settings() const;

 private:
  fs::path json_file_;
  fs::file_time_type last_write_time_;
  std::map<std::string, DictionarySettings> settings_;

  const std::string ENABLED = "enabled";
  const std::string DISABLED = "disabled";

  void loadJson();
  bool fileChanged();  // changes last_write_time_ !!

  bool isIn(const std::string& dictionary_info,
            const std::set<std::string>& vec);
};

}  // namespace dict

#endif  // TRANSLATORSSETTINGS_H
