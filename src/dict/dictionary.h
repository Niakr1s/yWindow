#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <string>

#include "card.h"

namespace dict {

class Dictionary {
 public:
  virtual ~Dictionary() {}

  Card query(const string& text) { return doQuery(text); }

 protected:
  virtual Card doQuery(const string& text) = 0;
};

class DefaultDictionary : public Dictionary {
 public:
  DefaultDictionary();

 protected:
  Card doQuery(const string& text) override;

 private:
};

}  // namespace dict

#endif  // DICTIONARY_H
