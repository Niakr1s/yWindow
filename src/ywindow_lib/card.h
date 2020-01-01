#ifndef CARD_H
#define CARD_H

#include <map>
#include <memory>
#include <string>
#include <vector>

namespace dict {

using string = std::string;

class Dictionary;

struct Tag {
  Tag();

  string tag;
  string keyword;
  int rating = 0;
  string description;
};

using TagMap = std::map<string, Tag>;

class Card {
 public:
  Card(Dictionary* dict);
  virtual ~Card();

  virtual string name() const = 0;
  virtual string reading() const = 0;
  virtual string meaning() const = 0;
  virtual string dictionaryInfo() const = 0;
  virtual string etc() const = 0;

  Dictionary* dict() const;

 protected:
  Dictionary* dict_;
};

class JsonCard : public Card {
 public:
  JsonCard(Dictionary* dict);

  void setName(const string& name);
  void setReading(const string& reading);
  void addTag(const string& tag);
  void setTags(std::vector<string>&& tags);
  void addMeaning(const string& meaning);

 public:
  string name_;
  string reading_;  // kun + on for kanji
  std::vector<string> tags_;
  std::vector<string> meanings_;

  // Card interface
 public:
  string name() const override;
  string reading() const override;
  string meaning() const override;
  string dictionaryInfo() const override;
  string etc() const override;
};

using CardPtrMap = std::multimap<string, std::unique_ptr<Card>>;
using CardPtrList = std::vector<Card*>;

}  // namespace dict

#endif  // CARD_H
