#ifndef TRANSLATIONCONVERTER_H
#define TRANSLATIONCONVERTER_H

#include <QString>
#include <map>
#include <set>
#include <vector>

#include "NLTemplate.h"
#include "card.h"
#include "translationresult.h"

using namespace NL::Template;

class TranslationConverter {
 public:
  virtual ~TranslationConverter();

  std::string toHtml(const dict::TranslationChunk& translation) {
    return doToHtml(translation);
  }

 protected:
  virtual std::string doToHtml(const dict::TranslationChunk& translation) = 0;
};

class YTranslationConverter : public TranslationConverter {
 public:
  YTranslationConverter() {}

  using word = std::string;
  using dict_info = std::string;
  using reading = std::string;
  using meanings = std::set<std::string>;

  using DictCardPtrMap =
      std::map<word, std::map<dict_info, std::map<reading, meanings>>>;

 protected:
  std::string doToHtml(const dict::TranslationChunk& translation) override;

 private:
  static DictCardPtrMap toDictCardPtrMap(const dict::CardPtrs& input,
                                         bool with_proxy = true);
  static void appendDictCardPtrMap(Block& word_block,
                                   const DictCardPtrMap& cards);
};

#endif  // TRANSLATIONCONVERTER_H
