#include "dictionary.h"

dict::Dictionary::Dictionary() {}

dict::Dictionary::~Dictionary() {}

dict::CardPtrList dict::Dictionary::query(const dict::string &text) {
  CardPtrList res;
  auto range = cards_.equal_range(text);
  for (auto card = range.first; card != range.second; ++card) {
    res.push_back(card->second.get());
  }
  return res;
}

dict::YomiDictionary::YomiDictionary()
    : Dictionary(), tags_(std::make_shared<TagMap>()) {}

void dict::Dictionary::updateInfo(const string &info) { info_ = info; }

void dict::YomiDictionary::addTag(const Tag &tag) { (*tags_)[tag.tag] = tag; }

void dict::Dictionary::addCard(Card *card) {
  auto card_to_insert = std::unique_ptr<Card>(card);
  cards_.insert({card_to_insert->name(), std::move(card_to_insert)});
}

const dict::string &dict::Dictionary::info() const { return info_; }

size_t dict::Dictionary::size() const { return cards_.size(); }

std::mutex &dict::Dictionary::mutex() { return mutex_; }

const dict::TagMap &dict::YomiDictionary::tags() const { return *tags_; }

const dict::CardPtrMap &dict::Dictionary::cards() const { return cards_; }
