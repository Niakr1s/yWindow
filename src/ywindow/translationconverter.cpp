#include "translationconverter.h"

#include <QDebug>
#include <set>
#include <sstream>

TranslationConverter::~TranslationConverter() {}

std::string YTranslationToHtmlConverter::doToHtml(
    const dict::TranslationChunk& translation) {
  auto cards = toDictCardPtrMap(translation.translations());
  auto sub_cards = toDictCardPtrMap(translation.subTranslations());

  qDebug() << "translations.size: " << translation.translations().size();

  LoaderFile loader;
  Template t(loader);
  t.load("templates/ytranslation.html");

  Block& word_block = t.block("word");
  word_block.repeat(cards.size() + sub_cards.size());

  appendDictCardPtrMap(word_block, cards, 0);
  appendDictCardPtrMap(word_block, sub_cards, cards.size());

  std::ostringstream ss;
  t.render(ss);
  std::string res = ss.str();
  qDebug() << QString::fromStdString(res);

  return res;
}

YTranslationToHtmlConverter::DictCardPtrMap
YTranslationToHtmlConverter::toDictCardPtrMap(
    const dict::CardPtrMultiMap& input) {
  std::set<dict::string> keys;
  for (auto& it : input) {
    keys.insert(it.first);
  }

  for (auto& i : input) {
    qDebug() << "toDictCardPtrMap" << QString::fromStdString(i.first)
             << QString::fromStdString(i.second->meaning()) << i.second;
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

void YTranslationToHtmlConverter::appendDictCardPtrMap(
    Block& word_block, const YTranslationToHtmlConverter::DictCardPtrMap& cards,
    size_t start) {
  size_t word_c = start;
  for (auto words_it = cards.cbegin(), words_it_max = cards.cend();
       words_it != words_it_max; ++words_it, ++word_c) {
    word_block[word_c].set("name", words_it->first);

    Block& dict_block = word_block[word_c].block("dict");
    dict_block.repeat(words_it->second.size());
    size_t dict_c = 0;
    for (auto dicts_it = words_it->second.cbegin(),
              dicts_it_max = words_it->second.cend();
         dicts_it != dicts_it_max; ++dicts_it, ++dict_c) {
      dict_block[dict_c].set("dict_info", dicts_it->first);

      Block& row_block = dict_block[dict_c].block("row");
      row_block.repeat(dicts_it->second.size());
      for (size_t cards_it = 0, cards_it_max = dicts_it->second.size();
           cards_it != cards_it_max; ++cards_it) {
        auto meaning = (dicts_it->second)[cards_it]->meaning();
        auto reading = (dicts_it->second)[cards_it]->reading();

        row_block[cards_it].set("meaning", meaning);
        row_block[cards_it].set("reading", reading);
      }
    }
  }
}
