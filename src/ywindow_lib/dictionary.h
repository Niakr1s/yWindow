#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <filesystem>
#include <memory>
#include <mutex>
#include <string>

#include "card.h"

namespace fs = std::filesystem;

namespace dict {

class Dictionary {
 public:
  Dictionary();
  virtual ~Dictionary();

  CardPtrs query(const std::string& text) const;

  virtual void addCard(std::shared_ptr<Card> card) {
    std::unique_lock<std::mutex> lock(mutex_);
    return doAddCard(card);
  }

  void updateInfo(const std::string& info) {
    std::unique_lock<std::mutex> lock(mutex_);
    return doUpdateInfo(info);
  }

  void makeProxyCards();

  virtual std::shared_ptr<std::string> info() const = 0;
  virtual size_t size() const = 0;

  std::mutex& mutex();

 protected:
  virtual CardPtrs doQuery(const std::string& text) const = 0;
  virtual void doAddCard(std::shared_ptr<Card> card) = 0;
  virtual void doUpdateInfo(const std::string& info) = 0;
  virtual void doMakeProxyCards() = 0;
  std::mutex mutex_;

 private:
  bool made_proxy_cards_ = false;
};

class DefaultDictionary : public Dictionary {
 public:
  DefaultDictionary();

  std::shared_ptr<std::string> info() const override;
  size_t size() const override;

 protected:
  std::shared_ptr<std::string> info_;
  CardPtrMap cards_;

  CardPtrs doQuery(const std::string& text) const override;
  virtual void doAddCard(std::shared_ptr<Card> card) override;
  void doUpdateInfo(const std::string& info) override;
  void doMakeProxyCards() override;
};

class YomiDictionary : public DefaultDictionary {
 public:
  YomiDictionary();

  void addTag(const Tag& tag);
  const TagMap& tags() const;

 protected:
  std::shared_ptr<TagMap> tags_;
};

class UserDictionary : public DefaultDictionary {
 public:
  UserDictionary();
};

class DeinflectDictionary : public DefaultDictionary {
 public:
  DeinflectDictionary();
};

}  // namespace dict

#endif  // DICTIONARY_H
