#ifndef TRANSLATIONCONVERTER_H
#define TRANSLATIONCONVERTER_H

#include <QString>
#include <map>
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

  using DictCardPtrMap =
      std::map<std::string,
               std::map<std::string, std::vector<std::shared_ptr<dict::Card>>>>;

 protected:
  std::string doToHtml(const dict::TranslationChunk& translation) override;

 private:
  static DictCardPtrMap toDictCardPtrMap(const dict::CardPtrs& input);
  static void appendDictCardPtrMap(Block& word_block,
                                   const DictCardPtrMap& cards);
};

#endif  // TRANSLATIONCONVERTER_H
