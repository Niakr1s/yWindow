#include "translationresult.h"

#include <algorithm>

bool dict::TranslationChunk::translated() const {
  return !translations_.empty();
}

dict::TranslationChunk::TranslationChunk() : TranslationChunk("", 0, 0) {}

dict::TranslationChunk::TranslationChunk(const std::string &text,
                                         size_t orig_begin, size_t orig_end)
    : text_(text), orig_begin_(orig_begin), orig_end_(orig_end) {}

std::string dict::TranslationChunk::text() const { return text_; }

size_t dict::TranslationChunk::orig_begin() const { return orig_begin_; }

size_t dict::TranslationChunk::orig_end() const { return orig_end_; }

dict::CardPtrMultiMap &dict::TranslationChunk::translations() {
  return translations_;
}

const dict::CardPtrMultiMap &dict::TranslationChunk::translations() const {
  return translations_;
}

dict::CardPtrMultiMap &dict::TranslationChunk::subTranslations() {
  return sub_translations_;
}

const dict::CardPtrMultiMap &dict::TranslationChunk::subTranslations() const {
  return sub_translations_;
}

dict::TranslationResult::TranslationResult(const std::string &orig_text)
    : orig_text_(orig_text) {}

dict::TranslationChunkPtrs &dict::TranslationResult::chunks() {
  return chunks_;
}

const dict::TranslationChunkPtrs &dict::TranslationResult::chunks() const {
  return chunks_;
}

// std::pair<dict::TranslationChunkPtr, size_t> dict::TranslationResult::chunk(
//    size_t orig_text_pos) const {
//  size_t counter = 0;
//  for (auto &chunk : chunks_) {
//    for (size_t i = 0, i_max = chunk->text().size(); i != i_max; ++i) {
//      if (counter++ == orig_text_pos) {
//        return {chunk, i};
//      }
//    }
//  }
//  return {nullptr, 0};
//}

void dict::TranslationResult::normalize() {
  sort();
  TranslationChunkPtrs new_chunks;
  size_t curr = 0;
  for (auto &chunk : chunks_) {
    if (curr >= orig_text_.size()) {
      break;
    }
    if (chunk->orig_begin() > curr) {
      auto new_chunk = std::shared_ptr<TranslationChunk>(new TranslationChunk{
          orig_text_.substr(curr, chunk->orig_begin() - curr), curr,
          chunk->orig_begin() - 1});
      new_chunks.push_back(new_chunk);
    }
    curr = chunk->orig_end() + 1;
  }
  if (curr != orig_text_.size()) {
    auto new_chunk = std::shared_ptr<TranslationChunk>(
        new TranslationChunk{orig_text_.substr(curr, orig_text_.size() - curr),
                             curr, orig_text_.size() - 1});
    new_chunks.push_back(new_chunk);
  }
  for (auto &new_chunk : new_chunks) {
    chunks_.push_back(new_chunk);
  }
  sort();
}

void dict::TranslationResult::sort() {
  std::sort(chunks_.begin(), chunks_.end(),
            [](TranslationChunkPtr lhs, TranslationChunkPtr rhs) {
              return lhs->orig_begin() < rhs->orig_begin();
            });
}

dict::TranslationResult dict::TranslatedText::mergeWith(
    const dict::TranslationResult &rhs) {
  TranslationResult res{orig_text};
  for (auto ch : rhs.chunks()) {
    auto beg_ch = chunk(ch->orig_begin());
    auto end_ch = chunk(ch->orig_end());

    auto orig_begin = beg_ch.first->chunk->orig_begin() + beg_ch.second;

    auto diff = end_ch.first->translated_text.size() - end_ch.second - 1;
    auto orig_end = end_ch.first->chunk->orig_end() - (diff > 0 ? diff : 0);

    auto subst = orig_text.substr(orig_begin, orig_end - orig_begin + 1);
    auto new_chunk =
        std::make_shared<TranslationChunk>(subst, orig_begin, orig_end);
    new_chunk->translations() = ch->translations();
    new_chunk->subTranslations() = ch->subTranslations();
    res.chunks().push_back(new_chunk);
  }
  res.normalize();
  return res;
}

std::pair<const dict::TranslatedTextChunk *, size_t>
dict::TranslatedText::chunk(size_t orig_text_pos) const {
  size_t counter = 0;
  for (auto &chunk : text) {
    for (size_t i = 0, i_max = chunk.translated_text.size(); i != i_max; ++i) {
      if (counter++ == orig_text_pos) {
        return {&chunk, i};
      }
    }
  }
  return {nullptr, 0};
}

std::string dict::TranslationResult::orig_text() const { return orig_text_; }

std::vector<dict::TranslatedText> dict::TranslationResult::translated_texts()
    const {
  return translated_texts_inner(TranslationResult(*this));
}

std::vector<dict::TranslatedText>
dict::TranslationResult::translated_texts_inner(TranslationResult input) {
  std::vector<TranslatedText> res;

  auto append_to_res = [&res, input](TranslatedTextChunk last_part) {
    auto inner_input = TranslationResult(input);
    inner_input.chunks().pop_back();
    if (inner_input.chunks().empty()) {
      res.push_back(TranslatedText{{last_part}, inner_input.orig_text()});
    } else {
      for (auto &s : translated_texts_inner(inner_input)) {
        auto to_push = s;
        to_push.text.push_back(last_part);
        res.push_back(to_push);
      }
    }
  };

  auto chunk = input.chunks().back();
  if (!chunk->translated()) {
    TranslatedTextChunk last_part_chunk;
    auto last_part = chunk->text();
    last_part_chunk.translated_text = last_part;
    last_part_chunk.chunk = chunk;
    append_to_res(last_part_chunk);
  } else {
    for (auto [s, card] : chunk->translations()) {
      for (auto &last_part : card->readings()) {
        TranslatedTextChunk last_part_chunk;
        last_part_chunk.translated_text = last_part;
        last_part_chunk.chunk = chunk;
        last_part_chunk.card = card;
        append_to_res(last_part_chunk);
      }
    }
  }

  return res;
}

std::string dict::TranslatedText::string() const {
  std::string res;
  for (auto &chunk : text) {
    res.append(chunk.translated_text);
  }
  return res;
}
