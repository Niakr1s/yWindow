#include "parser.h"

#include <json/json.h>

#include <boost/algorithm/string.hpp>
#include <fstream>

#include "card.h"
#include "dictionary.h"

void dict::Parser::parseInto(Dictionary *dict) { return doParseInto(dict); }

dict::Parser *dict::Parser::getParser(const fs::path &path) {
  if (fs::is_directory(path)) {
    throw std::invalid_argument("Parser::getJsonParser: path is directory");
  }
  if (path.extension() != ".json") {
    throw std::invalid_argument("Parser::getParser: path is not .json file");
  }
  auto ifs = new std::ifstream(path);
  if (path.filename() == "index.json") {
    return new JsonIndexParser(ifs);
  }
  auto stem = path.stem().string();
  if (stem.find("tag") != std::string::npos) {
    return new JsonTagParser(ifs);
  } else if (stem.find("term") != std::string::npos) {
    return new JsonTermParser(ifs);
  } else if (stem.find("kanji") != std::string::npos) {
    return new JsonKanjiParser(ifs);
  }
  return nullptr;
}

dict::JsonParser::JsonParser(std::istream *iss) : iss_(iss) {}

dict::JsonParser::~JsonParser() { delete iss_; }

Json::Value dict::JsonParser::getRoot() {
  Json::Value root;
  *iss_ >> root;
  return root;
}

dict::JsonIndexParser::JsonIndexParser(std::istream *iss) : JsonParser(iss) {}

dict::JsonTagParser::JsonTagParser(std::istream *iss) : JsonParser(iss) {}

dict::JsonTermParser::JsonTermParser(std::istream *iss) : JsonParser(iss) {}

dict::JsonKanjiParser::JsonKanjiParser(std::istream *iss) : JsonParser(iss) {}

void dict::JsonIndexParser::doParseInto(dict::Dictionary *dict) {
  Json::Value root = getRoot();
  Info index;

  index.title = root.get("title", "").asString();
  index.format = root.get("format", "0").asInt();
  index.revision = root.get("revision", "").asString();
  index.sequenced = root.get("sequenced", false).asBool();

  dict->updateIndex(index);
}

void dict::JsonTagParser::doParseInto(Dictionary *dict) {
  Json::Value root = getRoot();
  for (int i = 0, i_max = root.size(); i != i_max; ++i) {
    Tag tag;
    tag.tag = root[i][0].asString();
    tag.keyword = root[i][1].asString();
    tag.rating = root[i][2].asInt();
    tag.description = root[i][3].asString();
    dict->addTag(tag);
  }
}

void dict::JsonTermParser::doParseInto(dict::Dictionary *dict) {
  Json::Value root = getRoot();
  for (int i = 0, i_max = root.size(); i != i_max; ++i) {
    Card card;
    card.text = root[i][0].asString();

    card.reading = root[i][1].asString();

    boost::split(card.tags, root[i][2].asString(), boost::is_any_of(" "));

    card.rating = root[i][4].asInt();

    for (int j = 0, j_max = root[i][5].size(); j != j_max; ++j) {
      card.meanings.push_back(root[i][5][j].asString());
    }
    dict->addCard(card);
  }
}

void dict::JsonKanjiParser::doParseInto(dict::Dictionary *dict) {
  Json::Value root = getRoot();
  for (int i = 0, i_max = root.size(); i != i_max; ++i) {
    Card card;
    card.text = root[i][0].asString();

    std::vector<std::string> reading{root[i][1].asString(),
                                     root[i][2].asString()};
    card.reading = boost::join(reading, " ");

    boost::split(card.tags, root[i][3].asString(), boost::is_any_of(" "));

    for (int j = 0, j_max = root[i][4].size(); j != j_max; ++j) {
      card.meanings.push_back(root[i][4][j].asString());
    }
    dict->addCard(card);
  }
}
