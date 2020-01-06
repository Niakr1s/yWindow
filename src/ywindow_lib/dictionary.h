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

  CardPtrMultiMap query(const string& text) const;

  virtual void addCard(Card* card) {
    std::unique_lock<std::mutex> lock(mutex_);
    return doAddCard(card);
  }

  void updateInfo(const string& info) {
    std::unique_lock<std::mutex> lock(mutex_);
    return doUpdateInfo(info);
  }

  virtual const string& info() const = 0;
  virtual size_t size() const = 0;

  std::mutex& mutex();

 protected:
  virtual CardPtrMultiMap doQuery(const string& text) const = 0;
  virtual void doAddCard(Card* card) = 0;
  virtual void doUpdateInfo(const string& info) = 0;
  std::mutex mutex_;
};

class DefaultDictionary : public Dictionary {
 public:
  DefaultDictionary();

  const string& info() const override;
  size_t size() const override;

 protected:
  string info_;
  CardUniquePtrMap cards_;

  CardPtrMultiMap doQuery(const string& text) const override;
  virtual void doAddCard(Card* card) override;
  void doUpdateInfo(const string& info) override;
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
