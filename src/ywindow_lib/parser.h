#ifndef PARSER_H
#define PARSER_H

#include <json/json.h>

#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

namespace dict {

class Dictionary;

class Parser {
 public:
  virtual ~Parser() = default;

  void parseInto(Dictionary* dict);

  static Parser* getParser(const fs::path& path);

 protected:
  virtual void doParseInto(Dictionary* dict) = 0;
};

class JsonParser : public Parser {
 public:
  JsonParser(std::istream* iss);
  virtual ~JsonParser();

  Json::Value getRoot();

 protected:
  std::istream* iss_;
};

class JsonIndexParser : public JsonParser {
 public:
  JsonIndexParser(std::istream* iss);

 protected:
  void doParseInto(Dictionary* dict) override;
};

class JsonTagParser : public JsonParser {
 public:
  JsonTagParser(std::istream* iss);

 protected:
  void doParseInto(Dictionary* dict) override;
};

class JsonTermParser : public JsonParser {
 public:
  JsonTermParser(std::istream* iss);

 protected:
  void doParseInto(Dictionary* dict) override;
};

class JsonKanjiParser : public JsonParser {
 public:
  JsonKanjiParser(std::istream* iss);

 protected:
  void doParseInto(Dictionary* dict) override;
};

}  // namespace dict

#endif  // PARSER_H
