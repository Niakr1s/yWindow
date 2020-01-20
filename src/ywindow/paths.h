#ifndef PATHS_H
#define PATHS_H

#include <filesystem>

namespace fs = std::filesystem;

extern const fs::path Y_PATH;
extern const fs::path Y_TRANSLATORS_SETTINGS_PATH;
extern const fs::path Y_SETTINGS_PATH;

extern const fs::path Y_DICTS_PATH;

extern const fs::path Y_YOMI_DICTS_PATH;
extern const fs::path Y_DEINFLECT_DICTS_PATH;
extern const fs::path Y_USER_DICTS_PATH;

#endif  // PATHS_H
