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

  CardPtrList query(const string& text);

  void updateInfo(const string& info);

  void addCard(Card* card);
  const CardPtrMap& cards() const;

  const string& info() const;
  size_t size() const;

  std::mutex& mutex();

 protected:
  string info_;
  CardPtrMap cards_;
  std::mutex mutex_;
};

class YomiDictionary : public Dictionary {
 public:
  YomiDictionary();

  void addTag(const Tag& tag);
  const TagMap& tags() const;

 protected:
  std::shared_ptr<TagMap> tags_;
};

}  // namespace dict

#endif  // DICTIONARY_H
