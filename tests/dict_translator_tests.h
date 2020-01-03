#ifndef DICT_TRANSLATOR_TESTS_H
#define DICT_TRANSLATOR_TESTS_H

#include <gtest/gtest.h>

#include "dictionary.h"
#include "loader.h"
#include "translationresult.h"
#include "translator.h"

using namespace dict;

TEST(yomi_translator, test1) {
  auto yomi = new YomiTranslator("data");
  ASSERT_NO_THROW(yomi->translate("９日", false));
  auto res = yomi->translate("９日", false);
  ASSERT_EQ(res.orig_text(), "９日");
  ASSERT_EQ(res.chunks().size(), 1);
  ASSERT_EQ(res.chunks()[0].translations().size(), 2);
  ASSERT_TRUE(res.chunks()[0].translated());
}

TEST(yomi_translator, translation_result_all_false) {
  auto yomi = new YomiTranslator("data");
  auto res = yomi->translate("９日etc９日etc９日etc", false);
  ASSERT_EQ(res.chunks().size(), 2);
  ASSERT_TRUE(res.chunks()[0].translated());
  ASSERT_FALSE(res.chunks()[1].translated());
  ASSERT_EQ(res.chunks()[0].text(), "９日");
  ASSERT_EQ(res.chunks()[1].text(), "etc９日etc９日etc");
}

TEST(yomi_translator, translation_result_all_true) {
  auto yomi = new YomiTranslator("data");
  auto res = yomi->translate("９日etc９日etc９日etc", true);
  ASSERT_EQ(res.chunks().size(), 6);
  ASSERT_TRUE(res.chunks()[0].translated());
  ASSERT_FALSE(res.chunks()[1].translated());
  ASSERT_TRUE(res.chunks()[2].translated());
  ASSERT_FALSE(res.chunks()[3].translated());
  ASSERT_TRUE(res.chunks()[4].translated());
  ASSERT_FALSE(res.chunks()[5].translated());
  ASSERT_EQ(res.chunks()[0].text(), "９日");
  ASSERT_EQ(res.chunks()[1].text(), "etc");
  ASSERT_EQ(res.chunks()[2].text(), "９日");
  ASSERT_EQ(res.chunks()[3].text(), "etc");
  ASSERT_EQ(res.chunks()[4].text(), "９日");
  ASSERT_EQ(res.chunks()[5].text(), "etc");
}

#endif  // DICT_TRANSLATOR_TESTS_H