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

  static Parser* getParser(Dictionary* dict, const fs::path& path);

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

class YomiParser : public Parser {
  friend Parser;

 protected:
  YomiParser(std::istream* iss);
  virtual ~YomiParser();

  Json::Value getRoot();

 protected:
  std::istream* iss_;
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

}  // namespace dict

#endif  // PARSER_H
