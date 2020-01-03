#include "card.h"

#include <boost/algorithm/string.hpp>

#include "dictionary.h"

dict::YomiCard::YomiCard(Dictionary* dict)
    : DefaultCard(dict), yomi_dict_(dynamic_cast<YomiDictionary*>(dict)) {}

void dict::YomiCard::addTag(const dict::string& tag) { tags_.push_back(tag); }

void dict::YomiCard::setTags(std::vector<dict::string>&& tags) { tags_ = tags; }

dict::string dict::YomiCard::dictionaryInfo() const {
  return yomi_dict_->info();
}

void dict::YomiCard::addMeaning(const dict::string& meaning) {
  meanings_.push_back(meaning);
}

void dict::DefaultCard::setName(const string& name) { name_ = name; }

dict::Tag::Tag() {}

dict::DefaultCard::DefaultCard(Dictionary* dict) : dict_(dict) {}

dict::Card::~Card() {}

dict::Dictionary* dict::DefaultCard::dict() const { return dict_; }

dict::string dict::DefaultCard::name() const { return name_; }

dict::string dict::YomiCard::meaning() const {
  return boost::join(meanings_, "\n");
}

dict::string dict::YomiCard::etc() const {
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

dict::YomiKanjiCard::YomiKanjiCard(dict::Dictionary* dict) : YomiCard(dict) {}

void dict::YomiKanjiCard::setKunReading(const dict::string& reading) {
  kun_reading_ = reading;
}

void dict::YomiKanjiCard::setOnReading(const dict::string& reading) {
  on_reading_ = reading;
}

dict::string dict::YomiKanjiCard::reading() const {
  return string("Kun reading: " + kun_reading_ +
                "\nOn reading: " + on_reading_);
}

dict::YomiTermCard::YomiTermCard(dict::Dictionary* dict) : YomiCard(dict) {}

void dict::YomiTermCard::setReading(const dict::string& reading) {
  reading_ = reading;
}

dict::string dict::YomiTermCard::reading() const {
  return string("Reading: " + reading_);
}

dict::DeinflectCard::DeinflectCard(dict::Dictionary* dict)
    : DefaultCard(dict),
      deinflect_dict_(dynamic_cast<DeinflectDictionary*>(dict)) {}

void dict::DeinflectCard::setReading(const dict::string& meaning) {
  reading_ = meaning;
}

dict::string dict::DeinflectCard::reading() const { return reading_; }

dict::string dict::DeinflectCard::meaning() const { return ""; }

dict::string dict::DeinflectCard::dictionaryInfo() const {
  return deinflect_dict_->info();
}

dict::string dict::DeinflectCard::etc() const { return ""; }
