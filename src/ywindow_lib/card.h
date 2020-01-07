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

  // please, separate separate readings by ";"
  virtual std::string name() const = 0;
  virtual std::string reading() const = 0;
  virtual std::string meaning() const = 0;
  virtual std::string dictionaryInfo() const = 0;
  virtual std::string etc() const = 0;
};

class DefaultCard : public Card {
 public:
  DefaultCard(Dictionary* dict);

  void setName(const std::string& name);

  Dictionary* dict() const;

  std::string name() const override;

 protected:
  Dictionary* dict_;
  std::string name_;
};

class YomiDictionary;

class YomiCard : public DefaultCard {
 public:
  YomiCard(Dictionary* dict);

  void addTag(const std::string& tag);
  void setTags(std::vector<std::string>&& tags);
  void addMeaning(const std::string& meaning);

  std::string meaning() const override;
  std::string dictionaryInfo() const override;
  std::string etc() const override;

 protected:
  YomiDictionary* yomi_dict_;
  std::vector<std::string> meanings_;
  std::vector<std::string> tags_;
};

class YomiKanjiCard : public YomiCard {
 public:
  YomiKanjiCard(Dictionary* dict);

  void setKunReading(const std::string& reading);
  void setOnReading(const std::string& reading);

  std::string reading() const override;

 protected:
  std::string kun_reading_, on_reading_;
};

class YomiTermCard : public YomiCard {
 public:
  YomiTermCard(Dictionary* dict);

  void setReading(const std::string& reading);

  std::string reading() const override;

 protected:
  std::string reading_;
};

class DeinflectDictionary;

class DeinflectCard : public DefaultCard {
 public:
  DeinflectCard(Dictionary* dict);

  void setReading(const std::string& meaning);

  std::string reading() const override;
  std::string meaning() const override;
  std::string dictionaryInfo() const override;
  std::string etc() const override;

 private:
  std::string reading_;
  DeinflectDictionary* deinflect_dict_;
};

using CardUniquePtrMap = std::multimap<std::string, std::unique_ptr<Card>>;
using CardPtrMultiMap = std::multimap<std::string, Card*>;

}  // namespace dict

#endif  // CARD_H
