#include "translationresult.h"

#include <algorithm>

bool dict::TranslationChunk::translated() const {
  return !translations_.empty();
}

dict::TranslationChunk::TranslationChunk(const string &text, size_t orig_begin,
                                         size_t orig_end)
    : text_(text), orig_begin_(orig_begin), orig_end_(orig_end) {}

dict::string dict::TranslationChunk::text() const { return text_; }

size_t dict::TranslationChunk::orig_begin() const { return orig_begin_; }

size_t dict::TranslationChunk::orig_end() const { return orig_end_; }

dict::CardPtrMap &dict::TranslationChunk::translations() {
  return translations_;
}

const dict::CardPtrMap &dict::TranslationChunk::translations() const {
  return translations_;
}

dict::TranslationResult::TranslationResult(const dict::string &orig_text)
    : orig_text_(orig_text) {}

dict::TranslationChunks &dict::TranslationResult::chunks() { return chunks_; }

void dict::TranslationResult::normalize() {
  sort();
  TranslationChunks new_chunks;
  size_t curr = 0;
  for (auto &chunk : chunks_) {
    if (curr >= orig_text_.size()) {
      break;
    }
    if (chunk.orig_begin() > curr) {
      TranslationChunk new_chunk{
          orig_text_.substr(curr, chunk.orig_begin() - curr), curr,
          chunk.orig_begin() - 1};
      new_chunks.push_back(new_chunk);
    }
    curr = chunk.orig_end() + 1;
  }
  if (curr != orig_text_.size()) {
    TranslationChunk new_chunk{
        orig_text_.substr(curr, orig_text_.size() - curr), curr,
        orig_text_.size() - 1};
    new_chunks.push_back(new_chunk);
  }
  for (auto &new_chunk : new_chunks) {
    chunks_.push_back(new_chunk);
  }
  sort();
}

void dict::TranslationResult::sort() {
  std::sort(chunks_.begin(), chunks_.end(),
            [](const TranslationChunk &lhs, const TranslationChunk &rhs) {
              return lhs.orig_begin() < rhs.orig_begin();
            });
}

dict::string dict::TranslationResult::orig_text() const { return orig_text_; }
