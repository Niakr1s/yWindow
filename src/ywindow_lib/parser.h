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

  static Parser* getParser(Dictionary* dict, const fs::path& path);

 protected:
  virtual void doParseInto(Dictionary* dict) = 0;
};

class DummyParser : public Parser {
  friend Parser;

 protected:
  DummyParser();

 protected:
  void doParseInto(Dictionary*) override;
};

class JsonParser : public Parser {
 protected:
  JsonParser(std::istream* iss);
  virtual ~JsonParser();

  Json::Value getRoot();

 protected:
  std::istream* iss_;
};

class YomiParser : public JsonParser {
  friend Parser;

 protected:
  YomiParser(std::istream* iss);
};

class YomiIndexParser : public YomiParser {
  friend Parser;

 protected:
  YomiIndexParser(std::istream* iss);

 protected:
  void doParseInto(Dictionary* dict) override;
};

class YomiTagParser : public YomiParser {
  friend Parser;

 protected:
  YomiTagParser(std::istream* iss);

 protected:
  void doParseInto(Dictionary* dict) override;
};

class YomiTermParser : public YomiParser {
  friend Parser;

 protected:
  YomiTermParser(std::istream* iss);

 protected:
  void doParseInto(Dictionary* dict) override;
};

class YomiKanjiParser : public YomiParser {
  friend Parser;

 protected:
  YomiKanjiParser(std::istream* iss);

 protected:
  void doParseInto(Dictionary* dict) override;
};

class UserParser : public Parser {
  friend Parser;

 protected:
  UserParser(const fs::path& path);

  void doParseInto(Dictionary* dict) override;

 private:
  fs::path path_;
};

class DeinflectDictionary;

class DeinflectParser : public JsonParser {
  friend Parser;

 protected:
  DeinflectParser(std::istream* iss, const std::string& info);

 protected:
  std::string info_;

  void doParseInto(Dictionary* dict) override;
};

}  // namespace dict

#endif  // PARSER_H
