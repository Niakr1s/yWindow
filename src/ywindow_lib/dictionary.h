#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <filesystem>
#include <mutex>
#include <string>

#include "card.h"

namespace fs = std::filesystem;

namespace dict {

class Dictionary {
 public:
  Dictionary();
  virtual ~Dictionary();

  CardPtrList query(const string& text) const;

  std::mutex& mutex();

 protected:
  virtual CardPtrList doQuery(const string& text) const = 0;
  std::mutex mutex_;
};

class DefaultDictionary : public Dictionary {
 public:
  DefaultDictionary();

  void updateInfo(const string& info);

  void addCard(Card* card);
  const CardPtrMap& cards() const;

  const string& info() const;
  size_t size() const;

 protected:
  string info_;
  CardPtrMap cards_;

  CardPtrList doQuery(const string& text) const override;
};

class YomiDictionary : public DefaultDictionary {
 public:
  YomiDictionary();

  void addTag(const Tag& tag);
  const TagMap& tags() const;

 protected:
  std::shared_ptr<TagMap> tags_;
};

class DeinflectDictionary : public DefaultDictionary {
 public:
  DeinflectDictionary() : DefaultDictionary() {}
};

}  // namespace dict

#endif  // DICTIONARY_H
