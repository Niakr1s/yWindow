#ifndef JSONPARSER_H
#define JSONPARSER_H

#include <json/json.h>

#include <filesystem>

#include "card.h"

namespace fs = std::filesystem;

namespace dict {

struct JsonParserException : public std::exception {
  using std::exception::exception;
};

class JsonParser {
 public:
  struct Paths {
    fs::path dir;
    fs::path index_path;
    std::vector<fs::path> tag_paths, term_kanji_paths;
  };

  enum class Type { Kanji, Term, None };
  virtual ~JsonParser() {}

  static std::shared_ptr<JsonParser> makeParser(fs::path dir);

  CardMap parse();

 protected:
  JsonParser(Paths paths);

  virtual void doParseIndex(const fs::path& file_path, Index& index) = 0;
  virtual void doParseTagBank(const fs::path& file_path, TagMap& tag_map) = 0;
  virtual void doParseCardBank(const fs::path& file_path, CardMap& card_map,
                               std::shared_ptr<Index> index,
                               std::shared_ptr<TagMap> tag_map) = 0;

  Paths paths_;
};

class DefaultJsonParser : public JsonParser {
  friend class JsonParser;

 public:
 protected:
  DefaultJsonParser(Paths paths) : JsonParser(paths) {}

  void doParseIndex(const fs::path& file_path, Index& index) override;

  // [ "jouyou", "frequent", -5, "included in list of regular-use characters", 0
  // ] [ tag, keyword, rating, description, ... ]
  void doParseTagBank(const fs::path& file_path, TagMap& tag_map) override;

  Json::Value getRoot(const fs::path& file_path);
};

class KanjiJsonParser : public DefaultJsonParser {
 public:
  KanjiJsonParser(Paths paths) : DefaultJsonParser(paths) {}

 protected:
  void doParseCardBank(const fs::path& file_path, CardMap& card_map,
                       std::shared_ptr<Index> index,
                       std::shared_ptr<TagMap> tag_map) override;
};

class TermJsonParser : public DefaultJsonParser {
 public:
  TermJsonParser(Paths paths) : DefaultJsonParser(paths) {}

 protected:
  void doParseCardBank(const fs::path& file_path, CardMap& card_map,
                       std::shared_ptr<Index> index,
                       std::shared_ptr<TagMap> tag_map) override;
};

}  // namespace dict

#endif  // JSONPARSER_H
