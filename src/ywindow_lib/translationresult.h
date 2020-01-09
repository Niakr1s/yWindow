#ifndef TRANSLATIONRESULT_H
#define TRANSLATIONRESULT_H

#include <memory>

#include "card.h"

namespace dict {

// begin TranslationChunk

class TranslationChunk {
 public:
  TranslationChunk(const std::string& origin_text);
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
  TranslatedChunk(const std::string& origin_text)
      : TranslationChunk(origin_text) {}

  void insertTranslation(const std::pair<std::string, Card*>& transl) override;
  void insertSubTranslation(
      const std::pair<std::string, Card*>& transl) override;

  bool final() const override { return false; }
};

class TranslatedChunkFinal : public TranslatedChunk {
 public:
  TranslatedChunkFinal(const std::string& origin_text)
      : TranslatedChunk(origin_text) {}

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

  std::string orig_text() const;
  //  std::vector<TranslatedText> translated_texts() const;

  TranslationChunkPtrs& chunks();
  const TranslationChunkPtrs& chunks() const;

 private:
  std::string orig_text_;
  TranslationChunkPtrs chunks_;

  //  static std::vector<TranslatedText> translated_texts_inner(
  //      TranslationResult input);
};

}  // namespace dict

#endif  // TRANSLATIONRESULT_H
