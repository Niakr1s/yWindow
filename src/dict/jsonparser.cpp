#include "jsonparser.h"

#include <boost/algorithm/string.hpp>
#include <fstream>

dict::JsonParser::JsonParser(Paths paths) : paths_(paths) {}

std::shared_ptr<dict::JsonParser> dict::JsonParser::makeParser(fs::path dir) {
  if (!fs::is_directory(dir)) {
    throw JsonParserException("JsonParser::JsonParser: dir is not a directory");
  }

  Type type = Type::None;
  Paths paths;
  paths.dir = dir;
  for (auto &item : fs::directory_iterator(dir)) {
    fs::path p = item.path();
    if (!fs::is_regular_file(p) || p.extension() != "json") {
      continue;
    }
    std::string file_name = p.filename().string();
    if (p.filename() == "index.json") {
      paths.index_path = p;
      continue;
    }
    if (file_name.find("tag_bank") != std::string::npos) {
      paths.tag_paths.push_back(p);
      continue;
    }
    if (file_name.find("term_bank") != std::string::npos) {
      if (type == Type::Kanji) {
        throw JsonParserException(
            "JsonParser::makeParser: found term_bank, while already "
            "Type::Kanji");
      }
      type = Type::Term;
      paths.term_kanji_paths.push_back(p);
    } else if (file_name.find("kanji_bank") != std::string::npos) {
      if (type == Type::Term) {
        throw JsonParserException(
            "JsonParser::makeParser: found kanji_bank json, while already "
            "Type::Term");
      }
      type = Type::Kanji;
      paths.term_kanji_paths.push_back(p);
    }
  }
  switch (type) {
    case (Type::Kanji):
      return std::make_shared<KanjiJsonParser>(paths);
    case (Type::Term):
      return std::make_shared<TermJsonParser>(paths);
    case (Type::None):
      throw JsonParserException("JsonParser::makeParser: type is None");
  }
}

dict::CardMap dict::JsonParser::parse() {
  auto index = std::make_shared<Index>();
  auto tag_map = std::make_shared<TagMap>();
  CardMap card_map;

  doParseIndex(paths_.index_path, *index);

  for (auto &p : paths_.tag_paths) {
    doParseTagBank(p, *tag_map);
  }

  for (auto &p : paths_.term_kanji_paths) {
    doParseCardBank(p, card_map, index, tag_map);
  }
  return card_map;
}

void dict::DefaultJsonParser::doParseIndex(const fs::path &file_path,
                                           dict::Index &index) {
  Json::Value root = getRoot(file_path);

  index.title = root.get("title", "").asString();
  index.format = root.get("format", "0").asInt();
  index.revision = root.get("revision", "").asString();
  index.sequenced = root.get("sequenced", false).asBool();
}

void dict::DefaultJsonParser::doParseTagBank(const fs::path &file_path,
                                             dict::TagMap &tag_map) {
  Json::Value root = getRoot(file_path);

  for (int i = 0, i_max = root.size(); i != i_max; ++i) {
    Tag tag;
    tag.tag = root[i][0].asString();
    tag.keyword = root[i][1].asString();
    tag.rating = root[i][2].asInt();
    tag.description = root[i][3].asString();
    tag_map[tag.tag] = tag;
  }
}

Json::Value dict::DefaultJsonParser::getRoot(const fs::path &file_path) {
  Json::Value root;
  std::ifstream file_stream(file_path, std::ios_base::binary);
  file_stream >> root;
  return root;
}

void dict::KanjiJsonParser::doParseCardBank(const fs::path &file_path,
                                            dict::CardMap &card_map,
                                            std::shared_ptr<Index> index,
                                            std::shared_ptr<TagMap> tag_map) {
  Json::Value root = getRoot(file_path);

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

    card.index = index;
    card.tag_map = tag_map;

    card_map[card.text] = card;
  }
}

void dict::TermJsonParser::doParseCardBank(const fs::path &file_path,
                                           dict::CardMap &card_map,
                                           std::shared_ptr<Index> index,
                                           std::shared_ptr<TagMap> tag_map) {
  Json::Value root = getRoot(file_path);

  for (int i = 0, i_max = root.size(); i != i_max; ++i) {
    Card card;
    card.text = root[i][0].asString();

    card.reading = root[i][1].asString();

    boost::split(card.tags, root[i][2].asString(), boost::is_any_of(" "));

    card.rating = root[i][4].asInt();

    for (int j = 0, j_max = root[i][5].size(); j != j_max; ++j) {
      card.meanings.push_back(root[i][5][j].asString());
    }

    card.index = index;
    card.tag_map = tag_map;

    card_map[card.text] = card;
  }
}
