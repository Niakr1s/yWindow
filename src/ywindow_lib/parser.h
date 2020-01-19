#ifndef PARSER_H
#define PARSER_H

#include <json/json.h>

#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

namespace dict {

class Dictionary;
class YomiDictionary;

class Parser {
 public:
  virtual ~Parser() = default;

  void parseInto(Dictionary* dict);

  static Parser* getParser(Dictionary* dict, const fs::path& path,
                           bool only_info);

 protected:
  virtual void doParseInto(Dictionary* dict) = 0;
};

class DummyParser : public Parser {
  friend Parser;

 protected:
  DummyParser();

 protected:
  void doParseInto(Dictionary* dict) override;
};

class JsonParser : public Parser {
 protected:
  JsonParser(std::istream* iss, bool only_info);
  virtual ~JsonParser();

  Json::Value getRoot();

 protected:
  std::istream* iss_;
  bool only_info_;
};

class YomiParser : public JsonParser {
  friend Parser;

 protected:
  using JsonParser::JsonParser;
};

class YomiIndexParser : public YomiParser {
  friend Parser;

 protected:
  using YomiParser::YomiParser;

 protected:
  void doParseInto(Dictionary* dict) override;
};

class YomiTagParser : public YomiParser {
  friend Parser;

 protected:
  using YomiParser::YomiParser;

 protected:
  void doParseInto(Dictionary* dict) override;
};

class YomiTermParser : public YomiParser {
  friend Parser;

 protected:
  using YomiParser::YomiParser;

 protected:
  void doParseInto(Dictionary* dict) override;
};

class YomiKanjiParser : public YomiParser {
  friend Parser;

 protected:
  using YomiParser::YomiParser;

 protected:
  void doParseInto(Dictionary* dict) override;
};

class UserParser : public Parser {
  friend Parser;

 protected:
  UserParser(const fs::path& path, bool only_info);

  void doParseInto(Dictionary* dict) override;

 private:
  fs::path path_;
  bool only_info_;
};

class DeinflectDictionary;

class DeinflectParser : public JsonParser {
  friend Parser;

 protected:
  DeinflectParser(std::istream* iss, bool only_info, const std::string& info);

 protected:
  std::string info_;

  void doParseInto(Dictionary* dict) override;
};

}  // namespace dict

#endif  // PARSER_H
