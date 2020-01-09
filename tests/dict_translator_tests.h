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
  ASSERT_NO_THROW(yomi->translate("９日"));
  auto res = yomi->translate("９日");
  ASSERT_EQ(res.orig_text(), "９日");
  ASSERT_EQ(res.chunks().size(), 1);
  ASSERT_EQ(res.chunks()[0]->translations().size(), 2);
  ASSERT_TRUE(res.chunks()[0]->translated());
  ASSERT_EQ(res.chunks()[0]->subTranslations().size(), 1);
  ASSERT_TRUE(res.chunks()[0]->translated());
  ASSERT_TRUE(res.chunks()[0]->final());
}

TEST(yomi_translator, translation_result) {
  auto yomi = new YomiTranslator("data");
  auto res = yomi->translate("９日etc");
  ASSERT_EQ(res.chunks().size(), 4);
  ASSERT_TRUE(res.chunks()[0]->translated());
  ASSERT_FALSE(res.chunks()[1]->translated());
  ASSERT_EQ(res.chunks()[0]->originText(), "９日");
  ASSERT_EQ(res.chunks()[1]->originText(), "e");
  ASSERT_EQ(res.chunks()[2]->originText(), "t");
  ASSERT_EQ(res.chunks()[3]->originText(), "c");
}

TEST(yomi_translator, translation_result_all_true2) {
  auto yomi = new YomiTranslator("data");
  auto res = yomi->translate("見る笑う");
  ASSERT_EQ(res.chunks().size(), 2);
  ASSERT_TRUE(res.chunks()[0]->translated());
  ASSERT_TRUE(res.chunks()[1]->translated());
}

TEST(deinflector, test1) {
  auto de = new DeinflectTranslator("data/deinflect.json", nullptr);
  auto res = de->translate("見れば笑って");
  ASSERT_EQ(res.chunks().size(), 4);
  ASSERT_EQ(res.chunks()[0]->originText(), "見");
  ASSERT_EQ(res.chunks()[1]->originText(), "れば");
  ASSERT_EQ(res.chunks()[2]->originText(), "笑");
  ASSERT_EQ(res.chunks()[3]->originText(), "って");
  ASSERT_FALSE(res.chunks()[0]->translated());
  ASSERT_TRUE(res.chunks()[1]->translated());
  ASSERT_FALSE(res.chunks()[2]->translated());
  ASSERT_TRUE(res.chunks()[3]->translated());
  ASSERT_FALSE(res.chunks()[0]->final());
  ASSERT_FALSE(res.chunks()[1]->final());
  ASSERT_FALSE(res.chunks()[2]->final());
  ASSERT_FALSE(res.chunks()[3]->final());
}

TEST(usertranslator, test1) {
  auto de = new UserTranslator("data/user_dictionary.txt");
  auto res = de->translate("Niakr1s#1");
  ASSERT_EQ(res.chunks().size(), 3);
  ASSERT_EQ(res.chunks()[0]->originText(), "Niakr1s");
  ASSERT_EQ(res.chunks()[1]->originText(), "#");
  ASSERT_EQ(res.chunks()[2]->originText(), "1");
}

#endif  // DICT_TRANSLATOR_TESTS_H
