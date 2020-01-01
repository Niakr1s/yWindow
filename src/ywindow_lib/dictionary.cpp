#include "dictionary.h"

dict::Dictionary::~Dictionary() {}

dict::Card dict::Dictionary::query(const dict::string &text) {
  return doQuery(text);
}

void dict::Dictionary::updateIndex(const dict::Info &index) {
  return doUpdateIndex(index);
}

void dict::Dictionary::addTag(const dict::Tag &tag) { return doAddTag(tag); }

void dict::Dictionary::addCard(const dict::Card &card) {
  return doAddCard(card);
}

dict::Dictionary *dict::Dictionary::makeDefaultDictionary() {
  return new DefaultDictionary();
}

dict::DefaultDictionary::DefaultDictionary()
    : index_(std::make_shared<Info>()), tags_(std::make_shared<TagMap>()) {}

dict::Card dict::DefaultDictionary::doQuery(const dict::string &text) {
  Card res;
  return res;
}

void dict::DefaultDictionary::doUpdateIndex(const Info &index) {
  *index_ = index;
}

void dict::DefaultDictionary::doAddTag(const Tag &tag) {
  (*tags_)[tag.tag] = tag;
}

void dict::DefaultDictionary::doAddCard(const Card &card) {
  auto card_to_insert = card;
  card_to_insert.info_ = index_;
  card_to_insert.tag_map = tags_;
  cards_.insert({card_to_insert.text, card_to_insert});
}

const dict::Info &dict::DefaultDictionary::info() const { return *index_; }

const dict::TagMap &dict::DefaultDictionary::tags() const { return *tags_; }

const dict::CardMap &dict::DefaultDictionary::cards() const { return cards_; }
