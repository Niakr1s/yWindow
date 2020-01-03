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

  virtual void addCard(Card* card) = 0;
  virtual const CardPtrMap& cards() const = 0;

  virtual const string& info() const = 0;
  virtual void updateInfo(const string& info) = 0;
  virtual size_t size() const = 0;

  std::mutex& mutex();

 protected:
  virtual CardPtrList doQuery(const string& text) const = 0;
  std::mutex mutex_;
};

class DefaultDictionary : public Dictionary {
 public:
  DefaultDictionary();

  virtual void addCard(Card* card) override;
  const CardPtrMap& cards() const override;

  void updateInfo(const string& info) override;
  const string& info() const override;
  size_t size() const override;

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
