#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <string>

#include "card.h"

namespace dict {

class Dictionary {
 public:
  virtual ~Dictionary();

  Card query(const string& text);

  void updateIndex(const Info& info);
  void addTag(const Tag& tag);
  void addCard(const Card& card);

  virtual const Info& info() const = 0;
  virtual const TagMap& tags() const = 0;
  virtual const CardMap& cards() const = 0;

  static Dictionary* makeDefaultDictionary();

 protected:
  virtual Card doQuery(const string& text) = 0;

  virtual void doUpdateIndex(const Info& info) = 0;
  virtual void doAddTag(const Tag& tag) = 0;
  virtual void doAddCard(const Card& card) = 0;
};

class DefaultDictionary : public Dictionary {
 public:
  DefaultDictionary();

 protected:
  Card doQuery(const string& text) override;

  void doUpdateIndex(const Info& info) override;
  void doAddTag(const Tag& tag) override;
  void doAddCard(const Card& card) override;

  const Info& info() const override;
  const TagMap& tags() const override;
  const CardMap& cards() const override;

 private:
  std::shared_ptr<Info> index_;
  std::shared_ptr<TagMap> tags_;
  CardMap cards_;
};

}  // namespace dict

#endif  // DICTIONARY_H
