#ifndef TRANSLATIONTOHTMLCONVERTER_H
#define TRANSLATIONTOHTMLCONVERTER_H

#include <QString>
#include <map>
#include <vector>

#include "NLTemplate.h"
#include "card.h"
#include "translationresult.h"

using namespace NL::Template;

class TranslationToHtmlConverter {
 public:
  virtual ~TranslationToHtmlConverter();

  std::string convert(const dict::TranslationChunk& translation) {
    return doConvert(translation);
  }

 protected:
  virtual std::string doConvert(const dict::TranslationChunk& translation) = 0;
};

class YTranslationToHtmlConverter : public TranslationToHtmlConverter {
 public:
  YTranslationToHtmlConverter() {}

  using DictCardPtrMap =
      std::map<dict::string, std::map<dict::string, std::vector<dict::Card*>>>;

 protected:
  std::string doConvert(const dict::TranslationChunk& translation) override;

 private:
  static DictCardPtrMap toDictCardPtrMap(const dict::CardPtrMultiMap& input);
  static void appendDictCardPtrMap(Block& word_block,
                                   const DictCardPtrMap& cards, size_t start);
};

#endif  // TRANSLATIONTOHTMLCONVERTER_H
