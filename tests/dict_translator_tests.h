#ifndef DICT_TRANSLATOR_TESTS_H
#define DICT_TRANSLATOR_TESTS_H

#include <gtest/gtest.h>

#include "dictionary.h"
#include "loader.h"
#include "translator.h"

using namespace dict;

TEST(yomi_translator, test1) {
  auto yomi = new YomiTranslator("data");
  ASSERT_NO_THROW(yomi->translate(""));
}

#endif  // DICT_TRANSLATOR_TESTS_H
