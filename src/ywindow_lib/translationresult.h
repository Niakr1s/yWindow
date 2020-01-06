#ifndef TRANSLATIONRESULT_H
#define TRANSLATIONRESULT_H

#include "card.h"

namespace dict {

class TranslationChunk {
 public:
  TranslationChunk();
  TranslationChunk(const string& text, size_t orig_begin, size_t orig_end);

  string text() const;

  size_t orig_begin() const;
  size_t orig_end() const;

  bool translated() const;

  CardPtrMultiMap& translations();
  const CardPtrMultiMap& translations() const;

  CardPtrMultiMap& subTranslations();
  const CardPtrMultiMap& subTranslations() const;

 private:
  string text_;
  size_t orig_begin_, orig_end_;
  CardPtrMultiMap translations_, sub_translations_;
};

using TranslationChunks = std::vector<TranslationChunk>;

class TranslationResult {
 public:
  TranslationResult(const string& orig_text);

  string orig_text() const;

  TranslationChunks& chunks();

  // creating chunks for not translated parts of orig_text
  void normalize();

  void sort();

 private:
  string orig_text_;
  TranslationChunks chunks_;
};

}  // namespace dict

#endif  // TRANSLATIONRESULT_H
