#include "translationconverter.h"

#include <QDebug>
#include <boost/algorithm/string.hpp>
#include <set>
#include <sstream>

TranslationConverter::~TranslationConverter() {}

std::string YTranslationConverter::doToHtml(
    const dict::TranslationChunk& translation) {
  auto cards = toDictCardPtrMap(translation.translations(), false);
  if (cards.empty()) cards = toDictCardPtrMap(translation.translations(), true);

  auto sub_cards = toDictCardPtrMap(translation.subTranslations());

  qDebug() << "translations.size: " << translation.translations().size();

  LoaderFile loader;
  Template t(loader);
  t.load("yWindow/templates/ytranslation.html");

  appendDictCardPtrMap(t.block("cards"), cards);

  if (!sub_cards.empty()) {
    t.block("div").repeat(1);
    appendDictCardPtrMap(t.block("sub_cards"), sub_cards);
  } else {
    t.block("div").disable();
    t.block("sub_cards").disable();
  }

  std::ostringstream ss;
  t.render(ss);
  std::string res = ss.str();

  return res;
}

YTranslationConverter::DictCardPtrMap YTranslationConverter::toDictCardPtrMap(
    const dict::CardPtrs& input, bool with_proxy) {
  DictCardPtrMap res;
  for (auto& card : input) {
    if (!with_proxy && card->isProxy()) continue;
    res[card->originWord()][card->dictionaryInfo()][card->reading()].insert(
        card->meaning());
  }
  return res;
}

void YTranslationConverter::appendDictCardPtrMap(
    Block& block, const YTranslationConverter::DictCardPtrMap& cards) {
  block.repeat(cards.size());
  size_t word_c = 0;
  for (auto words_it = cards.cbegin(), words_it_max = cards.cend();
       words_it != words_it_max; ++words_it, ++word_c) {
    block[word_c].set("word", words_it->first);

    Block& dict_block = block[word_c].block("dict");
    dict_block.repeat(words_it->second.size());
    size_t dict_c = 0;
    for (auto dicts_it = words_it->second.cbegin();
         dicts_it != words_it->second.cend(); ++dicts_it, ++dict_c) {
      dict_block[dict_c].set("dict", dicts_it->first);

      Block& row_block = dict_block[dict_c].block("row");
      row_block.repeat(dicts_it->second.size());

      size_t row_c = 0;
      for (auto read_mean_it = dicts_it->second.cbegin();
           read_mean_it != dicts_it->second.cend(); ++read_mean_it, ++row_c) {
        row_block[row_c].set("reading", read_mean_it->first);
        Block& meaning_block = row_block[row_c].block("meaning");
        meaning_block.repeat(read_mean_it->second.size());

        size_t meaning_c = 0;
        for (auto meaning = read_mean_it->second.cbegin();
             meaning != read_mean_it->second.cend(); ++meaning, ++meaning_c) {
          meaning_block[meaning_c].set("meaning", *meaning);
        }
      }

      //      for (size_t cards_it = 0, cards_it_max = dicts_it->second.size();
      //           cards_it != cards_it_max; ++cards_it) {
      //        auto meaning = (dicts_it->second)[cards_it]->meaning();
      //        auto reading = (dicts_it->second)[cards_it]->reading();

      //        row_block[cards_it].set("meaning", meaning);
      //        row_block[cards_it].set("reading", reading);
      //      }
    }
  }
}
