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

void dict::DefaultCard::setWord(const std::string& name) { word_ = name; }

dict::Tag::Tag() {}

dict::DefaultCard::DefaultCard(Dictionary* dict) { dict_info_ = dict->info(); }

dict::Card::~Card() {}

std::string dict::Card::reading() { return boost::join(readings(), " "); }

std::string dict::Card::originWord() const { return word(); }

std::string dict::Card::etc() const { return ""; }

bool dict::Card::isProxy() const { return false; }

bool dict::Card::isKanji() const { return false; }

bool dict::Card::isTerm() const { return false; }

std::string dict::DefaultCard::dictionaryInfo() const { return *dict_info_; }

void dict::DefaultCard::setDictionaryInfo(std::shared_ptr<std::string> info) {
  dict_info_ = info;
}

std::string dict::DefaultCard::word() const { return word_; }

std::string dict::YomiCard::meaning() const {
  return boost::join(meanings_, "; ");
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

dict::UserCard::UserCard(dict::Dictionary* dict) : DefaultCard(dict) {}

void dict::UserCard::setReading(const std::string& reading) {
  reading_ = reading;
}

void dict::UserCard::setMeaning(const std::string& meaning) {
  meaning_ = meaning;
}

std::vector<std::string> dict::UserCard::readings() const { return {reading_}; }

std::string dict::UserCard::meaning() const { return meaning_; }

dict::ProxyCard::ProxyCard(std::shared_ptr<Card> card, const std::string& name)
    : card_(card), word_(name) {}

std::string dict::ProxyCard::originWord() const { return card_->word(); }

std::string dict::ProxyCard::word() const { return word_; }

std::vector<std::string> dict::ProxyCard::readings() const {
  return card_->readings();
}

std::string dict::ProxyCard::meaning() const { return card_->meaning(); }

std::string dict::ProxyCard::dictionaryInfo() const {
  return card_->dictionaryInfo();
}

std::string dict::ProxyCard::etc() const { return card_->etc(); }

void dict::ProxyCard::setDictionaryInfo(std::shared_ptr<std::string> info) {
  card_->setDictionaryInfo(info);
}

bool dict::ProxyCard::isProxy() const { return true; }

bool dict::ProxyCard::isTerm() const { return card_->isTerm(); }

bool dict::ProxyCard::isKanji() const { return card_->isKanji(); }

bool dict::YomiKanjiCard::isKanji() const { return true; }

bool dict::YomiTermCard::isTerm() const { return true; }
