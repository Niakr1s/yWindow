#ifndef TRANSLATIONRESULT_H
#define TRANSLATIONRESULT_H

#include <memory>

#include "card.h"

namespace dict {

// begin TranslationChunk

class TranslationChunk {
 public:
  TranslationChunk(const std::string& origin_text);
  TranslationChunk(const std::string& origin_text,
                   CardPtrMultiMap&& translations,
                   CardPtrMultiMap&& sub_translations);
  virtual ~TranslationChunk();

  std::string originText() const;
  std::string translatedText() const;

  bool translated() const;
  virtual bool final() const = 0;

  virtual void insertTranslation(const std::pair<std::string, Card*>&);
  const CardPtrMultiMap& translations() const;

  virtual void insertSubTranslation(const std::pair<std::string, Card*>&);
  const CardPtrMultiMap& subTranslations() const;

 protected:
  std::string origin_text_;
  CardPtrMultiMap translations_, sub_translations_;
};

class UntranslatedChunk : public TranslationChunk {
 public:
  UntranslatedChunk(const std::string& origin_text)
      : TranslationChunk(origin_text) {}

  bool final() const override { return false; }
};

class TranslatedChunk : public TranslationChunk {
 public:
  TranslatedChunk(const std::string& origin_text,
                  CardPtrMultiMap&& translations,
                  CardPtrMultiMap&& sub_translations)
      : TranslationChunk(origin_text, std::move(translations),
                         std::move(sub_translations)) {}

  bool final() const override { return false; }
};

class TranslatedChunkFinal : public TranslatedChunk {
 public:
  TranslatedChunkFinal(const std::string& origin_text,
                       CardPtrMultiMap&& translations,
                       CardPtrMultiMap&& sub_translations)
      : TranslatedChunk(origin_text, std::move(translations),
                        std::move(sub_translations)) {}

  bool final() const override { return true; }
};

// end TranslationChunk

using TranslationChunkPtr = std::shared_ptr<TranslationChunk>;
using TranslationChunkPtrs = std::vector<TranslationChunkPtr>;

// struct TranslatedTextChunk {
//  std::string translated_text;
//  TranslationChunkPtr chunk;
//  Card* card;
//};

// class TranslationResult;

// struct TranslatedText {
//  std::vector<TranslatedTextChunk> text;
//  std::string orig_text;
//  std::string string() const;
//  TranslationResult mergeWith(const TranslationResult& rhs);
//  std::pair<const TranslatedTextChunk*, size_t> chunk(
//      size_t orig_text_pos) const;
//};

class TranslationResult {
 public:
  TranslationResult(const std::string& orig_text);
  TranslationResult(TranslationChunkPtrs::const_iterator begin,
                    TranslationChunkPtrs::const_iterator end);

  std::string orig_text() const;
  size_t size() const;
  //  std::vector<TranslatedText> translated_texts() const;

  void insertChunk(TranslationChunkPtr chunk);
  const TranslationChunkPtrs& chunks() const;

 private:
  TranslationChunkPtrs chunks_;

  //  static std::vector<TranslatedText> translated_texts_inner(
  //      TranslationResult input);
};

}  // namespace dict

#endif  // TRANSLATIONRESULT_H
