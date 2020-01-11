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
  void setOriginText(const std::string& origin_text);

  bool translated() const;
  virtual bool final() const = 0;

  virtual void insertTranslation(const std::pair<std::string, Card*>&);
  const CardPtrMultiMap& translations() const;

  virtual void insertSubTranslation(const std::pair<std::string, Card*>&);
  const CardPtrMultiMap& subTranslations() const;

  virtual std::shared_ptr<TranslationChunk> copy() const = 0;

 protected:
  std::string origin_text_;
  CardPtrMultiMap translations_, sub_translations_;
};

class UntranslatedChunk : public TranslationChunk {
 public:
  UntranslatedChunk(const std::string& origin_text)
      : TranslationChunk(origin_text) {}

  bool final() const override { return false; }

  virtual std::shared_ptr<TranslationChunk> copy() const override {
    return std::make_shared<UntranslatedChunk>(*this);
  }
};

class TranslatedChunk : public TranslationChunk {
 public:
  TranslatedChunk(const std::string& origin_text);
  TranslatedChunk(const std::string& origin_text,
                  CardPtrMultiMap&& translations,
                  CardPtrMultiMap&& sub_translations);

  bool final() const override;

  virtual std::shared_ptr<TranslationChunk> copy() const override {
    return std::make_shared<TranslatedChunk>(*this);
  }
};

class TranslatedChunkFinal : public TranslatedChunk {
 public:
  TranslatedChunkFinal(const std::string& origin_text);
  TranslatedChunkFinal(const std::string& origin_text,
                       CardPtrMultiMap&& translations,
                       CardPtrMultiMap&& sub_translations);

  bool final() const override;
};

// end TranslationChunk

using TranslationChunkPtr = std::shared_ptr<TranslationChunk>;
using TranslationChunkPtrs = std::vector<TranslationChunkPtr>;

class TranslationText;

class TranslationTextChunk {
 public:
  TranslationTextChunk(TranslationChunkPtr chunk, const std::string& reading)
      : translation_chunk_(chunk), reading_(reading) {}

  std::string reading() const { return reading_; }

  TranslationText operator+(const TranslationText& rhs);

  TranslationChunkPtr translationChunk() const;

 private:
  TranslationChunkPtr translation_chunk_;
  std::string reading_;
};

class TranslationResult;

class TranslationText {
 public:
  TranslationText(std::vector<TranslationTextChunk>&& readings)
      : readings_(readings) {}

  std::string string() const;

  const std::vector<TranslationTextChunk>& readings() const;

  TranslationResult operator+(const TranslationResult& rhs);

 private:
  std::vector<TranslationTextChunk> readings_;
};

class TranslationResult {
 public:
  TranslationResult();
  TranslationResult(const std::string& orig_text);
  TranslationResult(TranslationChunkPtrs::const_iterator begin,
                    TranslationChunkPtrs::const_iterator end);
  TranslationResult(TranslationChunkPtrs&& chunks);

  std::string orig_text() const;
  size_t size() const;
  std::vector<TranslationText> toTexts() const;
  std::vector<TranslationResult> splitByFinal() const;
  bool final() const;

  void insertChunk(TranslationChunkPtr chunk);
  const TranslationChunkPtrs& chunks() const;

  TranslationChunkPtr chunk(int utf8_pos);

  TranslationResult operator+(const TranslationResult& rhs);
  void operator+=(const TranslationResult& rhs);

  size_t untranslatedSize() const;
  size_t translatedSize() const;

  std::vector<TranslationResult> mergeWithNextTranslation(
      const TranslationResult& next_transl_res);

  static TranslationResult bestTranslation(
      std::vector<TranslationResult> results);
  static TranslationResult join(
      const std::vector<TranslationResult>& transl_results);

 private:
  TranslationChunkPtrs chunks_;

  static std::vector<TranslationText> toTextsInner(TranslationResult input);
};

}  // namespace dict

#endif  // TRANSLATIONRESULT_H
