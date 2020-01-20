#ifndef PATHS_H
#define PATHS_H

#include <filesystem>

namespace fs = std::filesystem;

static const fs::path Y_PATH = "yWindow";
static const fs::path Y_TRANSLATORS_SETTINGS_PATH = Y_PATH / "dicts.json";
static const fs::path Y_SETTINGS_PATH = Y_PATH / "settings.ini";

static const fs::path Y_DICTS_PATH = Y_PATH / "dicts";

static const fs::path Y_YOMI_DICTS_PATH = Y_DICTS_PATH / "yomi";
static const fs::path Y_DEINFLECT_DICTS_PATH = Y_DICTS_PATH / "deinflect";
static const fs::path Y_USER_DICTS_PATH = Y_DICTS_PATH / "user";

#endif  // PATHS_H
