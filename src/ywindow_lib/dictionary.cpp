#include "dictionary.h"

dict::Dictionary::Dictionary() {}

dict::Dictionary::~Dictionary() {}

dict::CardPtrs dict::Dictionary::query(const std::string &text) const {
  return doQuery(text);
}

dict::DefaultDictionary::DefaultDictionary()
    : Dictionary(), info_(std::make_shared<std::string>()) {}

dict::YomiDictionary::YomiDictionary()
    : DefaultDictionary(), tags_(std::make_shared<TagMap>()) {}

void dict::DefaultDictionary::doUpdateInfo(const std::string &info) {
  *info_ = info;
}

void dict::YomiDictionary::addTag(const Tag &tag) {
  std::unique_lock<std::mutex> lock(mutex_);
  (*tags_)[tag.tag] = tag;
}

void dict::DefaultDictionary::doAddCard(std::shared_ptr<Card> card) {
  cards_.insert({card->word(), card});
}

std::shared_ptr<std::string> dict::DefaultDictionary::info() const {
  return info_;
}

size_t dict::DefaultDictionary::size() const { return cards_.size(); }

std::mutex &dict::Dictionary::mutex() { return mutex_; }

dict::CardPtrs dict::DefaultDictionary::doQuery(const std::string &text) const {
  CardPtrs res;
  auto range = cards_.equal_range(text);
  for (auto card = range.first; card != range.second; ++card) {
    res.push_back(card->second);
  }
  return res;
}

const dict::TagMap &dict::YomiDictionary::tags() const { return *tags_; }

dict::DeinflectDictionary::DeinflectDictionary() : DefaultDictionary() {}

dict::UserDictionary::UserDictionary() : DefaultDictionary() {}
