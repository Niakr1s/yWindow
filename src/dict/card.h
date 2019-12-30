#ifndef CARD_H
#define CARD_H

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace dict {

using string = std::string;

struct Index {
  Index();

  string title;
  int format;
  string revision;
  bool sequenced;
};

struct Tag {
  Tag();

  string tag;
  string keyword;
  int rating = 0;
  string description;
};

using TagMap = std::unordered_map<string, Tag>;

struct Card {
  Card();

  // card members from json
  string text;
  string reading;  // kun + on for kanji
  std::vector<string> tags;
  std::vector<string> meanings;

  // shared data from json
  std::shared_ptr<Index> index;
  std::shared_ptr<TagMap> tag_map;

  int rating = 0;
};

using CardMap = std::unordered_map<string, Card>;
using CardList = std::vector<Card>;

}  // namespace dict

#endif  // CARD_H
