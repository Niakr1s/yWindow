#ifndef DICT_QUERY_TESTS_H
#define DICT_QUERY_TESTS_H

#include <gtest/gtest.h>

#include "dictionary.h"
#include "loader.h"

using namespace dict;

TEST(defaultdict_query, test1) {
  auto future =
      Loader::loadFromFS<YomiDictionary>("data/yomi/kanjidic_english");
  auto dictionary = dynamic_cast<YomiDictionary*>(future.get());

  auto cards = dictionary->query("㝢");
  ASSERT_EQ(cards.size(), 0);

  cards = dictionary->query("見");
  ASSERT_EQ(cards.size(), 1);
}

#endif  // DICT_QUERY_TESTS_H
