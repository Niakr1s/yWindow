#ifndef CARD_H
#define CARD_H

#include <map>
#include <memory>
#include <string>
#include <vector>

namespace dict {

class Dictionary;

struct Tag {
  Tag();

  std::string tag;
  std::string keyword;
  int rating = 0;
  std::string description;
};

using TagMap = std::map<std::string, Tag>;

class Card {
 public:
  virtual ~Card();

  std::string reading();

  // please, separate separate readings by ";"
  virtual std::string originWord() const;
  virtual std::string word() const = 0;
  virtual std::vector<std::string> readings() const = 0;
  virtual std::string meaning() const = 0;
  virtual std::string dictionaryInfo() const = 0;
  virtual std::string etc() const = 0;
};

class DefaultCard : public Card {
 public:
  DefaultCard(Dictionary* dict);

  void setWord(const std::string& word);

  Dictionary* dict() const;
  std::string dictionaryInfo() const override;
  std::string word() const override;

 protected:
  Dictionary* dict_;
  std::string word_;
};

class YomiDictionary;

class YomiCard : public DefaultCard {
 public:
  YomiCard(Dictionary* dict);

  void addTag(const std::string& tag);
  void setTags(std::vector<std::string>&& tags);
  void addMeaning(const std::string& meaning);

  std::vector<std::string> readings() const override;
  std::string meaning() const override;
  std::string etc() const override;

  void setReading(const std::vector<std::string>& readings);

 protected:
  std::vector<std::string> meanings_;
  std::vector<std::string> tags_;
  std::vector<std::string> readings_;
};

class DeinflectDictionary;

class DeinflectCard : public DefaultCard {
 public:
  DeinflectCard(Dictionary* dict);

  void setReading(const std::string& reading);

  std::vector<std::string> readings() const override;
  std::string meaning() const override;
  std::string etc() const override;

 private:
  std::string reading_;
};

class UserCard : public DefaultCard {
 public:
  UserCard(Dictionary* dict);

  void setReading(const std::string& reading);

  std::vector<std::string> readings() const override;
  std::string meaning() const override;
  std::string etc() const override;

 private:
  std::string reading_;
};

class ProxyCard : public Card {
 public:
  ProxyCard(Card* card, const std::string& word);

  std::string originWord() const override;
  std::string word() const override;
  std::vector<std::string> readings() const override;
  std::string meaning() const override;
  std::string dictionaryInfo() const override;
  std::string etc() const override;

 private:
  Card* card_;
  std::string word_;
};

using CardUniquePtrMap = std::multimap<std::string, std::unique_ptr<Card>>;
using CardPtrMultiMap = std::multimap<std::string, Card*>;
using CardPtrs = std::vector<Card*>;

}  // namespace dict

#endif  // CARD_H
