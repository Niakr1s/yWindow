#include "parser.h"

#include <json/json.h>

#include <boost/algorithm/string.hpp>
#include <fstream>

#include "card.h"
#include "dictionary.h"

void dict::Parser::parseInto(Dictionary *dict) { return doParseInto(dict); }

dict::Parser *dict::Parser::getParser(Dictionary *dict, const fs::path &path) {
  if (fs::is_directory(path)) {
    throw std::invalid_argument("Parser::getJsonParser: path is directory");
  }
  if (YomiDictionary *yomi = dynamic_cast<YomiDictionary *>(dict); yomi) {
    auto ifs = new std::ifstream(path);
    if (path.filename() == "index.json") {
      return new YomiIndexParser(ifs);
    }
    auto stem = path.stem().string();
    if (stem.find("tag") != std::string::npos) {
      return new YomiTagParser(ifs);
    } else if (stem.find("term") != std::string::npos) {
      return new YomiTermParser(ifs);
    } else if (stem.find("kanji") != std::string::npos) {
      return new YomiKanjiParser(ifs);
    }
  }
  return new DummyParser();
}

dict::DummyParser::DummyParser() {}

void dict::DummyParser::doParseInto(Dictionary *) {}

dict::YomiParser::YomiParser(std::istream *iss) : iss_(iss) {}

dict::YomiParser::~YomiParser() { delete iss_; }

Json::Value dict::YomiParser::getRoot() {
  Json::Value root;
  *iss_ >> root;
  return root;
}

dict::YomiDictionary *dict::YomiParser::getYomi(dict::Dictionary *dict) {
  return dynamic_cast<YomiDictionary *>(dict);
}

dict::YomiIndexParser::YomiIndexParser(std::istream *iss) : YomiParser(iss) {}

dict::YomiTagParser::YomiTagParser(std::istream *iss) : YomiParser(iss) {}

dict::YomiTermParser::YomiTermParser(std::istream *iss) : YomiParser(iss) {}

dict::YomiKanjiParser::YomiKanjiParser(std::istream *iss) : YomiParser(iss) {}

void dict::YomiIndexParser::doParseInto(Dictionary *dict) {
  auto yomi = getYomi(dict);
  Json::Value root = getRoot();
  string info = root.get("title", "").asString();
  yomi->updateInfo(info);
}

void dict::YomiTagParser::doParseInto(Dictionary *dict) {
  auto yomi = getYomi(dict);
  Json::Value root = getRoot();
  for (int i = 0, i_max = root.size(); i != i_max; ++i) {
    Tag tag;
    tag.tag = root[i][0].asString();
    tag.keyword = root[i][1].asString();
    tag.rating = root[i][2].asInt();
    tag.description = root[i][3].asString();
    yomi->addTag(tag);
  }
}

void dict::YomiTermParser::doParseInto(Dictionary *dict) {
  auto yomi = getYomi(dict);
  Json::Value root = getRoot();
  for (int i = 0, i_max = root.size(); i != i_max; ++i) {
    YomiTermCard *card = new YomiTermCard(dict);
    card->setName(root[i][0].asString());

    card->setReading(root[i][1].asString());

    std::vector<string> tags;
    boost::split(tags, root[i][2].asString(), boost::is_any_of(" "));
    card->setTags(std::move(tags));

    for (int j = 0, j_max = root[i][5].size(); j != j_max; ++j) {
      card->addMeaning(root[i][5][j].asString());
    }

    yomi->addCard(card);
  }
}

void dict::YomiKanjiParser::doParseInto(Dictionary *dict) {
  auto yomi = getYomi(dict);
  Json::Value root = getRoot();
  for (int i = 0, i_max = root.size(); i != i_max; ++i) {
    YomiKanjiCard *card = new YomiKanjiCard(dict);
    card->setName(root[i][0].asString());

    card->setKunReading(root[i][1].asString());
    card->setOnReading(root[i][2].asString());

    std::vector<string> tags;
    boost::split(tags, root[i][3].asString(), boost::is_any_of(" "));
    card->setTags(std::move(tags));

    for (int j = 0, j_max = root[i][4].size(); j != j_max; ++j) {
      card->addMeaning(root[i][4][j].asString());
    }
    yomi->addCard(card);
  }
}
