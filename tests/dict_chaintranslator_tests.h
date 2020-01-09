#ifndef DICT_CHAINTRANSLATOR_TESTS_H
#define DICT_CHAINTRANSLATOR_TESTS_H

#include <gtest/gtest.h>

#include <string>

#include "translationresult.h"
#include "translator.h"

using namespace dict;

TEST(chaintranslator, test1) {
  ChainTranslator chain;
  chain.addTranslator(new UserTranslator("data/user_dictionary.txt"));
  chain.addTranslator(new DeinflectTranslator("data/deinflect.json", nullptr));
  chain.addTranslator(new YomiTranslator("data"));
  auto res = chain.translate("見ればNiakr1s笑って");
  ASSERT_EQ(res.size(), 3);
  ASSERT_EQ(res.chunks()[0]->translations().begin()->first, "見る");
  ASSERT_EQ(res.chunks()[1]->translations().begin()->first, "Niakr1s");
  ASSERT_EQ(res.chunks()[1]->translations().begin()->second->readings().front(),
            "Nia");
  ASSERT_EQ(res.chunks()[2]->translations().begin()->first, "笑う");

  res = chain.translate("見れば笑ってNiakr1s");
  ASSERT_EQ(res.size(), 3);
  ASSERT_EQ(res.chunks()[0]->translations().begin()->first, "見る");
  ASSERT_EQ(res.chunks()[2]->translations().begin()->first, "Niakr1s");
  ASSERT_EQ(res.chunks()[2]->translations().begin()->second->readings().front(),
            "Nia");
  ASSERT_EQ(res.chunks()[1]->translations().begin()->first, "笑う");

  res = chain.translate("Niakr1s見れば笑って");
  ASSERT_EQ(res.size(), 3);
  ASSERT_EQ(res.chunks()[1]->translations().begin()->first, "見る");
  ASSERT_EQ(res.chunks()[0]->translations().begin()->first, "Niakr1s");
  ASSERT_EQ(res.chunks()[0]->translations().begin()->second->readings().front(),
            "Nia");
  ASSERT_EQ(res.chunks()[2]->translations().begin()->first, "笑う");

  res = chain.translate("見Niakr1sれば笑Niakr1sって");
  ASSERT_EQ("", "");
}

#endif  // DICT_CHAINTRANSLATOR_TESTS_H
