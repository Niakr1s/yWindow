#include "card.h"

#include <boost/algorithm/string.hpp>

#include "dictionary.h"

dict::YomiCard::YomiCard(Dictionary* dict) : DefaultCard(dict) {}

void dict::YomiCard::addTag(const std::string& tag) { tags_.push_back(tag); }

void dict::YomiCard::setTags(std::vector<std::string>&& tags) { tags_ = tags; }

void dict::YomiCard::addMeaning(const std::string& meaning) {
  meanings_.push_back(meaning);
}

std::vector<std::string> dict::YomiCard::readings() const { return readings_; }

void dict::DefaultCard::setName(const std::string& name) { name_ = name; }

dict::Tag::Tag() {}

dict::DefaultCard::DefaultCard(Dictionary* dict) : dict_(dict) {}

dict::Card::~Card() {}

std::string dict::Card::reading() { return boost::join(readings(), " "); }

std::string dict::Card::originName() const { return name(); }

dict::Dictionary* dict::DefaultCard::dict() const { return dict_; }

std::string dict::DefaultCard::dictionaryInfo() const { return dict_->info(); }

std::string dict::DefaultCard::name() const { return name_; }

std::string dict::YomiCard::meaning() const {
  return boost::join(meanings_, "; ");
}

std::string dict::YomiCard::etc() const {
  std::string res;
  auto yomi_dict = dynamic_cast<YomiDictionary*>(dict_);
  if (!yomi_dict) return res;

  std::vector<std::string> to_append;
  for (auto& tag : tags_) {
    try {
      to_append.push_back(tag + ": " + yomi_dict->tags().at(tag).description);
    } catch (...) {
    }
  }
  res.append(boost::join(to_append, "; "));

  return res;
}

void dict::YomiCard::setReading(const std::vector<std::string>& reading) {
  readings_ = reading;
}

dict::DeinflectCard::DeinflectCard(dict::Dictionary* dict)
    : DefaultCard(dict) {}

void dict::DeinflectCard::setReading(const std::string& reading) {
  reading_ = reading;
}

std::vector<std::string> dict::DeinflectCard::readings() const {
  return std::vector<std::string>{reading_};
}

std::string dict::DeinflectCard::meaning() const { return ""; }

std::string dict::DeinflectCard::etc() const { return ""; }

dict::UserCard::UserCard(dict::Dictionary* dict) : DefaultCard(dict) {}

void dict::UserCard::setReading(const std::string& reading) {
  reading_ = reading;
}

std::vector<std::string> dict::UserCard::readings() const { return {reading_}; }

std::string dict::UserCard::meaning() const { return name_; }

std::string dict::UserCard::etc() const { return ""; }

dict::ProxyCard::ProxyCard(dict::Card* card, const std::string& name)
    : card_(card), name_(name) {}

std::string dict::ProxyCard::originName() const { return card_->name(); }

std::string dict::ProxyCard::name() const { return name_; }

std::vector<std::string> dict::ProxyCard::readings() const {
  return card_->readings();
}

std::string dict::ProxyCard::meaning() const { return card_->meaning(); }

std::string dict::ProxyCard::dictionaryInfo() const {
  return card_->dictionaryInfo();
}

std::string dict::ProxyCard::etc() const { return card_->etc(); }
