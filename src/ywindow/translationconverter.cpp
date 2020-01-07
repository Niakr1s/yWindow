#include "translationconverter.h"

#include <QDebug>
#include <set>
#include <sstream>

TranslationConverter::~TranslationConverter() {}

std::string YTranslationConverter::doToHtml(
    const dict::TranslationChunk& translation) {
  auto cards = toDictCardPtrMap(translation.translations());
  auto sub_cards = toDictCardPtrMap(translation.subTranslations());

  qDebug() << "translations.size: " << translation.translations().size();

  LoaderFile loader;
  Template t(loader);
  t.load("templates/ytranslation.html");

  appendDictCardPtrMap(t.block("cards"), cards);
  appendDictCardPtrMap(t.block("sub_cards"), cards);

  std::ostringstream ss;
  t.render(ss);
  std::string res = ss.str();
  qDebug() << QString::fromStdString(res);

  return res;
}

YTranslationConverter::DictCardPtrMap YTranslationConverter::toDictCardPtrMap(
    const dict::CardPtrMultiMap& input) {
  std::set<std::string> keys;
  for (auto& it : input) {
    keys.insert(it.first);
  }

  DictCardPtrMap res;
  for (auto& k : keys) {
    auto range = input.equal_range(k);
    for (auto it = range.first; it != range.second; ++it) {
      res[k][it->second->dictionaryInfo()].push_back(it->second);
    }
  }

  return res;
}

void YTranslationConverter::appendDictCardPtrMap(
    Block& block, const YTranslationConverter::DictCardPtrMap& cards) {
  block.repeat(cards.size());
  size_t word_c = 0;
  for (auto words_it = cards.cbegin(), words_it_max = cards.cend();
       words_it != words_it_max; ++words_it, ++word_c) {
    block[word_c].set("name", words_it->first);

    Block& dict_block = block[word_c].block("dict");
    dict_block.repeat(words_it->second.size());
    size_t dict_c = 0;
    for (auto dicts_it = words_it->second.cbegin(),
              dicts_it_max = words_it->second.cend();
         dicts_it != dicts_it_max; ++dicts_it, ++dict_c) {
      dict_block[dict_c].set("dict", dicts_it->first);

      Block& row_block = dict_block[dict_c].block("row");
      row_block.repeat(dicts_it->second.size());
      for (size_t cards_it = 0, cards_it_max = dicts_it->second.size();
           cards_it != cards_it_max; ++cards_it) {
        auto meaning = (dicts_it->second)[cards_it]->meaning();
        auto reading = (dicts_it->second)[cards_it]->reading();

        row_block[cards_it].set(
            "colspan", reading.empty() ? std::to_string(2) : std::to_string(1));
        row_block[cards_it].set("width", reading.empty() ? std::to_string(100)
                                                         : std::to_string(80));

        row_block[cards_it].set("meaning", meaning);

        Block& reading_block = row_block[cards_it].block("reading");
        if (!reading.empty()) {
          reading_block.set("reading", reading);
        } else {
          reading_block.disable();
        }
      }
    }
  }
}
