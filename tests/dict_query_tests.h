#ifndef DICT_QUERY_TESTS_H
#define DICT_QUERY_TESTS_H

#include <gtest/gtest.h>

#include "dictionary.h"
#include "loader.h"

using namespace dict;

TEST(defaultdict_query, test1) {
  auto future =
      Loader::loadFromFS<YomiDictionary>("data/kanjidic_english");
  auto dictionary = dynamic_cast<YomiDictionary*>(future.get());

  std::string to_query = "㝢";
  auto cards = dictionary->query("㝢");
  ASSERT_EQ(cards.size(), 1);
}

#endif  // DICT_QUERY_TESTS_H
