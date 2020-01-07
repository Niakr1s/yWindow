#ifndef TRANSLATIONRESULT_H
#define TRANSLATIONRESULT_H

#include <memory>

#include "card.h"

namespace dict {

class TranslationChunk {
 public:
  TranslationChunk();
  TranslationChunk(const std::string& text, size_t orig_begin, size_t orig_end);

  std::string text() const;

  size_t orig_begin() const;
  size_t orig_end() const;

  bool translated() const;

  CardPtrMultiMap& translations();
  const CardPtrMultiMap& translations() const;

  CardPtrMultiMap& subTranslations();
  const CardPtrMultiMap& subTranslations() const;

 private:
  std::string text_;
  size_t orig_begin_, orig_end_;
  CardPtrMultiMap translations_, sub_translations_;
};

using TranslationChunkPtr = std::shared_ptr<TranslationChunk>;
using TranslationChunkPtrs = std::vector<TranslationChunkPtr>;

struct TranslatedTextChunk {
  std::string translated_text;
  TranslationChunkPtr chunk;
  Card* card;
};

class TranslationResult;

struct TranslatedText {
  std::vector<TranslatedTextChunk> text;
  std::string orig_text;
  std::string string() const;
  TranslationResult mergeWith(const TranslationResult& rhs);
  std::pair<const TranslatedTextChunk*, size_t> chunk(
      size_t orig_text_pos) const;
};

class TranslationResult {
 public:
  TranslationResult(const std::string& orig_text);

  std::string orig_text() const;
  std::vector<TranslatedText> translated_texts() const;

  TranslationChunkPtrs& chunks();
  const TranslationChunkPtrs& chunks() const;

  // creating chunks for not translated parts of orig_text
  void normalize();
  void sort();

 private:
  std::string orig_text_;
  TranslationChunkPtrs chunks_;

  static std::vector<TranslatedText> translated_texts_inner(
      TranslationResult input);
};

}  // namespace dict

#endif  // TRANSLATIONRESULT_H
