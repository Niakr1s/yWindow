#ifndef CARD_H
#define CARD_H

#include <map>
#include <memory>
#include <string>
#include <vector>

namespace dict {

using string = std::string;

struct Info {
  Info();

  string title = "";
  int format = 0;
  string revision = "";
  bool sequenced = false;
};

struct Tag {
  Tag();

  string tag;
  string keyword;
  int rating = 0;
  string description;
};

using TagMap = std::map<string, Tag>;

struct Card {
  Card();

  // card members from json
  string text;
  string reading;  // kun + on for kanji
  std::vector<string> tags;
  std::vector<string> meanings;

  // shared data from json
  std::shared_ptr<Info> info_;
  std::shared_ptr<TagMap> tag_map;

  int rating = 0;
};

using CardMap = std::multimap<string, Card>;
using CardList = std::vector<Card>;

}  // namespace dict

#endif  // CARD_H
