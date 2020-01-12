#ifndef TRANSLATOR_H
#define TRANSLATOR_H

#include <filesystem>
#include <future>
#include <memory>
#include <string>
#include <vector>

#include "dictionary.h"
#include "translationresult.h"

namespace fs = std::filesystem;

namespace dict {

class Translator {
 public:
  virtual ~Translator();

  TranslationResult translate(const std::wstring& wstr);
  TranslationResult translate(const std::string& str);

 protected:
  virtual TranslationResult doTranslate(const std::string& str) = 0;
  virtual void prepareDictionaries();

  static size_t MAX_CHUNK_SIZE;
};

class DictionaryTranslator : public Translator {
 public:
  DictionaryTranslator(Translator* deinflector = nullptr);

  void addDict(Dictionary* dict);
  void setDeinflector(Translator* deinflector);

 protected:
  // in children provide dicts via dicts_futures_ or dicts_
  std::vector<std::unique_ptr<Dictionary>> dicts_;
  std::vector<std::future<Dictionary*>> dicts_futures_;
  std::unique_ptr<Translator> deinflector_;

  CardPtrs queryAllDicts(const std::string& str);

  void prepareDictionaries() final;

  // template main function, use it with TranslatedChunk or TranslatedChunkFinal
  template <class TranslatedChunk_T>
  TranslationResult doTranslateAll(const std::string& str);

 private:
  // second arg of pair is num of first full utf8 symbols translated
  // equal to num of first translated chunks() in TranslationResult{str}
  template <class TranslatedChunk_T>
  std::pair<TranslationChunkPtr, int> doTranslateFirstOf(
      const std::string& str);

  template <class TranslatedChunk_T>
  TranslationChunkPtr doTranslateFullStr(const std::string& str);

  template <class TranslatedChunk_T>
  TranslationChunkPtr doDeinflectAndTranslateFullStr(const std::string& str);
};

class YomiTranslator : public DictionaryTranslator {
 public:
  YomiTranslator(const fs::path& root_dir, Translator* deinflector = nullptr);
  YomiTranslator(std::initializer_list<fs::path> dicts_dirs,
                 Translator* deinflector = nullptr);

 protected:
  TranslationResult doTranslate(const std::string& str) override;
};

class DeinflectTranslator : public DictionaryTranslator {
 public:
  DeinflectTranslator(const fs::path& file, Translator* next_translator);

 protected:
  TranslationResult doTranslate(const std::string& str) override;

 private:
  Translator* next_translator_;
};

class UserTranslator : public DictionaryTranslator {
 public:
  UserTranslator(const fs::path& file);

 protected:
  TranslationResult doTranslate(const std::string& str);
};

class ChainTranslator : public Translator {
 public:
  ChainTranslator();
  ChainTranslator(std::initializer_list<Translator*> translators);

  void addTranslator(Translator* transl);
  void popTranslator();

 protected:
  TranslationResult doTranslate(const std::string& str);

 private:
  std::vector<std::unique_ptr<Translator>> translators_;
};

}  // namespace dict

#endif  // TRANSLATOR_H
