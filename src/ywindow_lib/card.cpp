#include "card.h"

#include <boost/algorithm/string.hpp>

#include "dictionary.h"

dict::JsonCard::JsonCard(Dictionary* dict) : Card(dict) {}

void dict::JsonCard::setReading(const string& reading) { reading_ = reading; }

void dict::JsonCard::addTag(const dict::string& tag) { tags_.push_back(tag); }

void dict::JsonCard::setTags(std::vector<dict::string>&& tags) { tags_ = tags; }

void dict::JsonCard::addMeaning(const dict::string& meaning) {
  meanings_.push_back(meaning);
}

void dict::JsonCard::setName(const string& name) { name_ = name; }

dict::Tag::Tag() {}

dict::Card::Card(dict::Dictionary* dict) : dict_(dict) {}

dict::Card::~Card() {}

dict::Dictionary* dict::Card::dict() const { return dict_; }

dict::string dict::JsonCard::name() const { return name_; }

dict::string dict::JsonCard::reading() const { return reading_; }

dict::string dict::JsonCard::meaning() const {
  return boost::join(meanings_, " ");
}

dict::string dict::JsonCard::dictionaryInfo() const { return dict_->info(); }

dict::string dict::JsonCard::etc() const {
  string res;

  res.append("Tags:\n");

  std::vector<string> to_append;
  for (auto& tag : tags_) {
    to_append.push_back(
        tag + ": " +
        dynamic_cast<YomiDictionary*>(dict_)->tags().at(tag).description);
  }
  res.append(boost::join(to_append, "\n"));

  return res;
}
