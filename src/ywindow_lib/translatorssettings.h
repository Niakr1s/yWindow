#ifndef TRANSLATORSSETTINGS_H
#define TRANSLATORSSETTINGS_H

#include <filesystem>
#include <map>
#include <set>

namespace fs = std::filesystem;

namespace dict {

class Translator;

struct DictionaryInfo {
  std::string dictionary_info;
  bool enabled;
  fs::path path;

  inline bool operator==(const DictionaryInfo& rhs) const {
    return dictionary_info == rhs.dictionary_info;
  }

  inline bool operator==(const std::string& rhs) const {
    return dictionary_info == rhs;
  }

  inline bool operator<(const DictionaryInfo& rhs) const {
    return dictionary_info < rhs.dictionary_info;
  }
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
  void addDictionary(const std::string& translator_info,
                     const std::string& dictionary_info, const fs::path& path,
                     bool enabled = true);
  void enableDictionary(const std::string& translator_info,
                        const std::string& dictionary_info);
  void disableDictionary(const std::string& translator_info,
                         const std::string& dictionary_info);
  void moveDictionary(const std::string& translator_info,
                      const std::string& dictionary_info, bool enabled);
  void deleteDictionary(const std::string& translator_info,
                        const std::string& dictionary_info);

  void updateDictionaryPath(const std::string& translator_info,
                            const std::string& dictionary_info,
                            const fs::path& path);

  fs::path getDictionaryPath(const std::string& translator_info,
                             const std::string& dictionary_info);

  // deletes dictionaries NOT IN dictionary_infos
  void deleteOtherDictionaries(const std::string& translator_info,
                               const std::set<std::string>& dictionary_infos);
  void saveJson();

  int size() const;

  const std::map<std::string, std::set<DictionaryInfo>>& settings() const;

 private:
  fs::path json_file_;
  fs::file_time_type last_write_time_;
  std::map<std::string, std::set<DictionaryInfo>> settings_;

  const std::string ENABLED = "enabled";
  const std::string DISABLED = "disabled";

  void loadJson();
  bool fileChanged();  // changes last_write_time_ !!

  bool isIn(const std::string& dictionary_info,
            const std::set<std::string>& vec);

  DictionaryInfo& findDictionaryInfo(const std::string& translator_info,
                                     const std::string& dictionary_info);
};

}  // namespace dict

#endif  // TRANSLATORSSETTINGS_H
