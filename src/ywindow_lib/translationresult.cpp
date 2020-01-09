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

void dict::TranslationChunk::setOriginText(const std::string &origin_text) {
  origin_text_ = origin_text;
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

std::vector<dict::TranslationText> dict::TranslationResult::toTexts() const {
  return toTextsInner(TranslationResult(*this));
}

std::vector<dict::TranslationText> dict::TranslationResult::toTextsInner(
    TranslationResult transl_res) {
  std::vector<TranslationText> res;
  if (transl_res.size() == 0) return res;

  auto it = transl_res.chunks().begin(), it_end = transl_res.chunks().end();

  if ((*it)->translated()) {
    for (auto &[_, card] : (*it)->translations()) {
      for (auto &reading : card->readings()) {
        TranslationTextChunk first(*it, reading);

        TranslationResult remained_transl_res{it + 1, it_end};
        auto remained_texts = toTextsInner(remained_transl_res);
        if (remained_texts.empty()) {
          res.push_back(TranslationText{{first}});
        } else {
          for (auto &remained_readings : remained_texts) {
            res.push_back(TranslationText{first + remained_readings});
          }
        }
      }
    }
  } else {
    TranslationTextChunk first(*it, (*it)->originText());

    TranslationResult remained_transl_res{it + 1, it_end};
    auto remained_texts = toTextsInner(remained_transl_res);
    if (remained_texts.empty()) {
      res.push_back(TranslationText{{first}});
    } else {
      for (auto &remained_readings : remained_texts) {
        res.push_back(TranslationText{first + remained_readings});
      }
    }
  }

  return res;
}

std::string dict::TranslationText::string() const {
  std::string res;
  for (auto &reading : readings_) {
    res.append(reading.reading());
  }
  return res;
}

const std::vector<dict::TranslationTextChunk> &dict::TranslationText::readings()
    const {
  return readings_;
}

dict::TranslationResult dict::TranslationText::operator+(
    const dict::TranslationResult &rhs) {
  TranslationChunkPtrs res_chunks;
  auto it = readings_.begin(), it_end = readings_.end();
  for (auto rhs_chunk : rhs.chunks()) {
    auto rhs_chunk_orig_text = rhs_chunk->originText();
    std::string reading, origin_text;
    while (it != it_end && reading != rhs_chunk_orig_text) {
      reading.append(it->reading());
      origin_text.append(it->translationChunk()->originText());
      ++it;
    }
    rhs_chunk->setOriginText(origin_text);
    res_chunks.push_back(rhs_chunk);
  }
  return TranslationResult(rhs.chunks().begin(), rhs.chunks().end());
}

dict::TranslationText dict::TranslationTextChunk::operator+(
    const dict::TranslationText &rhs) {
  std::vector<TranslationTextChunk> new_readings{{*this}};
  new_readings.insert(new_readings.end(), rhs.readings().cbegin(),
                      rhs.readings().cend());
  return TranslationText(std::move(new_readings));
}

dict::TranslationChunkPtr dict::TranslationTextChunk::translationChunk() const {
  return translation_chunk_;
}

dict::TranslatedChunk::TranslatedChunk(const std::string &origin_text,
                                       dict::CardPtrMultiMap &&translations,
                                       dict::CardPtrMultiMap &&sub_translations)
    : TranslationChunk(origin_text, std::move(translations),
                       std::move(sub_translations)) {}

bool dict::TranslatedChunk::final() const { return false; }

dict::TranslatedChunkFinal::TranslatedChunkFinal(
    const std::string &origin_text, dict::CardPtrMultiMap &&translations,
    dict::CardPtrMultiMap &&sub_translations)
    : TranslatedChunk(origin_text, std::move(translations),
                      std::move(sub_translations)) {}

bool dict::TranslatedChunkFinal::final() const { return true; }
