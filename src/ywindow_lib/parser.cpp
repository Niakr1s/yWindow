#include "parser.h"

#include <json/json.h>

#include <boost/algorithm/string.hpp>
#include <fstream>

#include "card.h"
#include "dictionary.h"
#include "stringconvert.h"

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
  } else if (auto deinflect = dynamic_cast<DeinflectDictionary *>(dict);
             deinflect) {
    auto ifs = new std::ifstream(path);
    return new DeinflectParser(ifs, path.filename().string());
  } else if (auto user = dynamic_cast<UserDictionary *>(dict); user) {
    return new UserParser(path);
  }
  return new DummyParser();
}

dict::DummyParser::DummyParser() {}

void dict::DummyParser::doParseInto(Dictionary *) {}

dict::JsonParser::JsonParser(std::istream *iss) : iss_(iss) {}

dict::JsonParser::~JsonParser() { delete iss_; }

Json::Value dict::JsonParser::getRoot() {
  Json::Value root;
  *iss_ >> root;
  return root;
}

dict::YomiParser::YomiParser(std::istream *iss) : JsonParser(iss) {}

dict::YomiIndexParser::YomiIndexParser(std::istream *iss) : YomiParser(iss) {}

dict::YomiTagParser::YomiTagParser(std::istream *iss) : YomiParser(iss) {}

dict::YomiTermParser::YomiTermParser(std::istream *iss) : YomiParser(iss) {}

dict::YomiKanjiParser::YomiKanjiParser(std::istream *iss) : YomiParser(iss) {}

void dict::YomiIndexParser::doParseInto(Dictionary *dict) {
  Json::Value root = getRoot();
  std::string info = root.get("title", "").asString();
  dict->updateInfo(info);
}

void dict::YomiTagParser::doParseInto(Dictionary *dict) {
  auto yomi = dynamic_cast<YomiDictionary *>(dict);
  if (!yomi) {
    return;
  }
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
  Json::Value root = getRoot();
  for (int i = 0, i_max = root.size(); i != i_max; ++i) {
    YomiCard *card = new YomiCard(dict);
    card->setWord(root[i][0].asString());

    auto json_readings = root[i][1].asString();
    if (json_readings.empty()) {
      card->setReading({card->word()});
    } else {
      std::vector<std::string> readings;
      boost::split(readings, root[i][1].asString(), boost::is_any_of(" "));
      card->setReading(readings);
    }

    auto json_tags = root[i][2].asString();
    if (!json_tags.empty()) {
      std::vector<std::string> tags;
      boost::split(tags, root[i][2].asString(), boost::is_any_of(" "));
      card->setTags(std::move(tags));
    }

    for (int j = 0, j_max = root[i][5].size(); j != j_max; ++j) {
      card->addMeaning(root[i][5][j].asString());
    }

    dict->addCard(card);

    for (auto &reading : card->readings()) {
      if (reading != card->word()) {
        ProxyCard *reading_card = new ProxyCard(card, reading);
        dict->addCard(reading_card);
      }
    }
  }
}

void dict::YomiKanjiParser::doParseInto(Dictionary *dict) {
  Json::Value root = getRoot();
  for (int i = 0, i_max = root.size(); i != i_max; ++i) {
    YomiCard *card = new YomiCard(dict);
    card->setWord(root[i][0].asString());

    auto json_readings = root[i][1].asString() + " " + root[i][2].asString();
    boost::trim(json_readings);
    if (json_readings.empty()) {
      card->setReading({card->word()});
    } else {
      std::vector<std::string> readings;
      boost::split(readings, json_readings, boost::is_any_of(" "));
      card->setReading(readings);
    }

    auto json_tags = root[i][3].asString();
    if (!json_tags.empty()) {
      std::vector<std::string> tags;
      boost::split(tags, root[i][3].asString(), boost::is_any_of(" "));
      card->setTags(std::move(tags));
    }

    for (int j = 0, j_max = root[i][4].size(); j != j_max; ++j) {
      card->addMeaning(root[i][4][j].asString());
    }
    dict->addCard(card);

    for (auto &reading : card->readings()) {
      if (reading != card->word()) {
        ProxyCard *reading_card = new ProxyCard(card, reading);
        dict->addCard(reading_card);
      }
    }
  }
}

dict::DeinflectParser::DeinflectParser(std::istream *iss,
                                       const std::string &info)
    : JsonParser(iss), info_(info) {}

void dict::DeinflectParser::doParseInto(dict::Dictionary *dict) {
  Json::Value root = getRoot();
  dict->updateInfo(info_);
  for (auto it = root.begin(), it_max = root.end(); it != it_max; ++it) {
    for (int i = 0, i_max = it->size(); i != i_max; ++i) {
      DeinflectCard *card = new DeinflectCard(dict);
      card->setWord((*it)[i]["kanaIn"].asString());
      card->setReading((*it)[i]["kanaOut"].asString());
      dict->addCard(card);
    }
  }
}

dict::UserParser::UserParser(const fs::path &path) : Parser(), path_(path) {}

void dict::UserParser::doParseInto(dict::Dictionary *dict) {
  std::ifstream is(path_);
  dict->updateInfo(path_.filename().string());
  if (!is.is_open()) {
    return;
  }

  for (std::string line; std::getline(is, line);) {
    if (line.empty() || line[0] == '#') continue;

    std::vector<std::string> pair;
    boost::split(pair, line, boost::is_any_of("="));
    trimStr(pair);

    if (pair.size() != 2) continue;

    auto card = new UserCard(dict);
    card->setWord(pair[0]);

    std::vector<std::string> reading_and_meaning;
    boost::split(reading_and_meaning, pair[1], boost::is_any_of(","));
    trimStr(reading_and_meaning);

    if (reading_and_meaning.empty()) continue;

    card->setReading(reading_and_meaning[0]);

    if (reading_and_meaning.size() == 2) {
      card->setMeaning(reading_and_meaning[1]);
    } else {
      card->setMeaning(card->word());
    }

    dict->addCard(card);
  }
}
