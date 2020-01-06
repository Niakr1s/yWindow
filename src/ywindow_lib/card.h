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
  virtual ~Card();

  // please, separate separate readings by ";"
  virtual string name() const = 0;
  virtual string reading() const = 0;
  virtual string meaning() const = 0;
  virtual string dictionaryInfo() const = 0;
  virtual string etc() const = 0;
};

class DefaultCard : public Card {
 public:
  DefaultCard(Dictionary* dict);

  void setName(const string& name);

  Dictionary* dict() const;

  string name() const override;

 protected:
  Dictionary* dict_;
  string name_;
};

class YomiDictionary;

class YomiCard : public DefaultCard {
 public:
  YomiCard(Dictionary* dict);

  void addTag(const string& tag);
  void setTags(std::vector<string>&& tags);
  void addMeaning(const string& meaning);

  string meaning() const override;
  string dictionaryInfo() const override;
  string etc() const override;

 protected:
  YomiDictionary* yomi_dict_;
  std::vector<string> meanings_;
  std::vector<string> tags_;
};

class YomiKanjiCard : public YomiCard {
 public:
  YomiKanjiCard(Dictionary* dict);

  void setKunReading(const string& reading);
  void setOnReading(const string& reading);

  string reading() const override;

 protected:
  string kun_reading_, on_reading_;
};

class YomiTermCard : public YomiCard {
 public:
  YomiTermCard(Dictionary* dict);

  void setReading(const string& reading);

  string reading() const override;

 protected:
  string reading_;
};

class DeinflectDictionary;

class DeinflectCard : public DefaultCard {
 public:
  DeinflectCard(Dictionary* dict);

  void setReading(const string& meaning);

  string reading() const override;
  string meaning() const override;
  string dictionaryInfo() const override;
  string etc() const override;

 private:
  string reading_;
  DeinflectDictionary* deinflect_dict_;
};

using CardUniquePtrMap = std::multimap<string, std::unique_ptr<Card>>;
using CardPtrMultiMap = std::multimap<string, Card*>;

}  // namespace dict

#endif  // CARD_H
