#include "translationresult.h"

#include <algorithm>

#include "utf8.h"

bool dict::TranslationChunk::translated() const {
  return !translations_.empty();
}

void dict::TranslationChunk::insertTranslation(
    const std::pair<std::string, dict::Card *> &) {}

dict::TranslationChunk::TranslationChunk(const std::string &origin_text)
    : origin_text_(origin_text) {}

dict::TranslationChunk::TranslationChunk(
    const std::string &origin_text, dict::CardPtrMultiMap &&translations,
    dict::CardPtrMultiMap &&sub_translations)
    : origin_text_(origin_text),
      translations_(translations),
      sub_translations_(sub_translations) {}

dict::TranslationChunk::~TranslationChunk() {}

std::string dict::TranslationChunk::originText() const { return origin_text_; }

const dict::CardPtrMultiMap &dict::TranslationChunk::translations() const {
  return translations_;
}

void dict::TranslationChunk::insertSubTranslation(
    const std::pair<std::string, dict::Card *> &) {}

const dict::CardPtrMultiMap &dict::TranslationChunk::subTranslations() const {
  return sub_translations_;
}

dict::TranslationResult::TranslationResult(const std::string &orig_text) {
  if (auto it = utf8::find_invalid(orig_text.begin(), orig_text.end());
      it != orig_text.end()) {
    throw std::runtime_error("TranslationResult: not valid utf8 string");
  }
  for (auto it = orig_text.begin(), it_prev = orig_text.begin(),
            it_end = orig_text.end();
       it != it_end; it_prev = it) {
    utf8::next(it, it_end);
    chunks_.push_back(
        std::make_shared<UntranslatedChunk>(std::string(it_prev, it)));
  }
}

dict::TranslationResult::TranslationResult(
    TranslationChunkPtrs::const_iterator begin,
    TranslationChunkPtrs::const_iterator end)
    : chunks_(begin, end) {}

// dict::TranslationChunkPtrs &dict::TranslationResult::chunks() {
//  return chunks_;
//}

const dict::TranslationChunkPtrs &dict::TranslationResult::chunks() const {
  return chunks_;
}

std::string dict::TranslationResult::orig_text() const {
  std::string res;
  for (auto &ch : chunks_) {
    res.append(ch->originText());
  }
  return res;
}

size_t dict::TranslationResult::size() const { return chunks_.size(); }

void dict::TranslationResult::insertChunk(dict::TranslationChunkPtr chunk) {
  chunks_.push_back(chunk);
}

// dict::TranslationResult dict::TranslatedText::mergeWith(
//    const dict::TranslationResult &rhs) {
//  TranslationResult res{orig_text};
//  for (auto ch : rhs.chunks()) {
//    auto beg_this_ch = chunk(ch->orig_begin());
//    auto end_this_ch = chunk(ch->orig_end());

//    auto orig_begin =
//        beg_this_ch.first->chunk->orig_begin() + beg_this_ch.second;

//    auto diff =
//        end_this_ch.first->translated_text.size() - end_this_ch.second - 1;
//    auto orig_end =
//        end_this_ch.first->chunk->orig_end() - (diff > 0 ? diff : 0);

//    auto subst = orig_text.substr(orig_begin, orig_end - orig_begin + 1);
//    auto new_chunk =
//        std::make_shared<TranslationChunk>(subst, orig_begin, orig_end);
//    new_chunk->translations() = ch->translations();
//    new_chunk->subTranslations() = ch->subTranslations();
//    res.chunks().push_back(new_chunk);
//  }
//  res.normalize();
//  return res;
//}

// std::pair<const dict::TranslatedTextChunk *, size_t>
// dict::TranslatedText::chunk(size_t orig_text_pos) const {
//  size_t counter = 0;
//  for (auto &chunk : text) {
//    for (size_t i = 0, i_max = chunk.translated_text.size(); i != i_max; ++i)
//    {
//      if (counter++ == orig_text_pos) {
//        return {&chunk, i};
//      }
//    }
//  }
//  return {nullptr, 0};
//}

// std::vector<dict::TranslatedText> dict::TranslationResult::translated_texts()
//    const {
//  return translated_texts_inner(TranslationResult(*this));
//}

// std::vector<dict::TranslatedText>
// dict::TranslationResult::translated_texts_inner(TranslationResult input) {
//  std::vector<TranslatedText> res;

//  auto append_to_res = [&res, input](TranslatedTextChunk last_part) {
//    auto inner_input = TranslationResult(input);
//    inner_input.chunks().pop_back();
//    if (inner_input.chunks().empty()) {
//      res.push_back(TranslatedText{{last_part}, inner_input.orig_text()});
//    } else {
//      for (auto &s : translated_texts_inner(inner_input)) {
//        auto to_push = s;
//        to_push.text.push_back(last_part);
//        res.push_back(to_push);
//      }
//    }
//  };

//  auto chunk = input.chunks().back();
//  if (!chunk->translated()) {
//    TranslatedTextChunk last_part_chunk;
//    auto last_part = chunk->text();
//    last_part_chunk.translated_text = last_part;
//    last_part_chunk.chunk = chunk;
//    append_to_res(last_part_chunk);
//  } else {
//    for (auto [s, card] : chunk->translations()) {
//      for (auto &last_part : card->readings()) {
//        TranslatedTextChunk last_part_chunk;
//        last_part_chunk.translated_text = last_part;
//        last_part_chunk.chunk = chunk;
//        last_part_chunk.card = card;
//        append_to_res(last_part_chunk);
//      }
//    }
//  }

//  return res;
//}

// std::string dict::TranslatedText::string() const {
//  std::string res;
//  for (auto &chunk : text) {
//    res.append(chunk.translated_text);
//  }
//  return res;
//}
