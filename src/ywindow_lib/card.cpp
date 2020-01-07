#include "card.h"

#include <boost/algorithm/string.hpp>

#include "dictionary.h"

dict::YomiCard::YomiCard(Dictionary* dict)
    : DefaultCard(dict), yomi_dict_(dynamic_cast<YomiDictionary*>(dict)) {}

void dict::YomiCard::addTag(const std::string& tag) { tags_.push_back(tag); }

void dict::YomiCard::setTags(std::vector<std::string>&& tags) { tags_ = tags; }

std::string dict::YomiCard::dictionaryInfo() const {
  return yomi_dict_->info();
}

void dict::YomiCard::addMeaning(const std::string& meaning) {
  meanings_.push_back(meaning);
}

std::string dict::YomiCard::reading() const { return readings_; }

void dict::DefaultCard::setName(const std::string& name) { name_ = name; }

dict::Tag::Tag() {}

dict::DefaultCard::DefaultCard(Dictionary* dict) : dict_(dict) {}

dict::Card::~Card() {}

dict::Dictionary* dict::DefaultCard::dict() const { return dict_; }

std::string dict::DefaultCard::name() const { return name_; }

std::string dict::YomiCard::meaning() const {
  return boost::join(meanings_, "; ");
}

std::string dict::YomiCard::etc() const {
  std::string res;

  std::vector<std::string> to_append;
  for (auto& tag : tags_) {
    try {
      to_append.push_back(tag + ": " + yomi_dict_->tags().at(tag).description);
    } catch (...) {
    }
  }
  res.append(boost::join(to_append, "; "));

  return res;
}

void dict::YomiCard::setReading(const std::string& reading) {
  readings_ = reading;
}

dict::DeinflectCard::DeinflectCard(dict::Dictionary* dict)
    : DefaultCard(dict),
      deinflect_dict_(dynamic_cast<DeinflectDictionary*>(dict)) {}

void dict::DeinflectCard::setReading(const std::string& meaning) {
  reading_ = meaning;
}

std::string dict::DeinflectCard::reading() const { return reading_; }

std::string dict::DeinflectCard::meaning() const { return ""; }

std::string dict::DeinflectCard::dictionaryInfo() const {
  return deinflect_dict_->info();
}

std::string dict::DeinflectCard::etc() const { return ""; }
