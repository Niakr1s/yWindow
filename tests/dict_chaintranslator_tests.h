#ifndef DICT_CHAINTRANSLATOR_TESTS_H
#define DICT_CHAINTRANSLATOR_TESTS_H

#include <gtest/gtest.h>

#include <string>

#include "translationresult.h"
#include "translator.h"

using namespace dict;

TEST(chaintranslator, test1) {
  ChainTranslator chain;
  chain.addTranslator(new UserTranslator("data/user"));
  chain.addTranslator(new DeinflectTranslator("data/deinflect.json"));
  chain.addTranslator(new YomiTranslator("data/yomi"));
  TranslationResult res;

  res = chain.translate("見れば笑ってNiakr1s");
  ASSERT_EQ(res.size(), 3);
  ASSERT_EQ(res.chunks()[0]->translations().front()->word(), "見る");
  ASSERT_EQ(res.chunks()[0]->originText(), "見れば");
  ASSERT_EQ(res.chunks()[2]->translations().front()->word(), "Niakr1s");
  ASSERT_EQ(res.chunks()[2]->translations().front()->readings().front(), "Nia");
  ASSERT_EQ(res.chunks()[1]->translations().front()->word(), "笑う");
  ASSERT_EQ(res.chunks()[1]->originText(), "笑って");

  res = chain.translate("見ればNiakr1s笑って");
  ASSERT_EQ(res.size(), 3);
  ASSERT_EQ(res.chunks()[0]->translations().front()->word(), "見る");
  ASSERT_EQ(res.chunks()[0]->originText(), "見れば");
  ASSERT_EQ(res.chunks()[1]->translations().front()->word(), "Niakr1s");
  ASSERT_EQ(res.chunks()[1]->translations().front()->readings().front(), "Nia");
  ASSERT_EQ(res.chunks()[2]->translations().front()->word(), "笑う");
  ASSERT_EQ(res.chunks()[2]->originText(), "笑って");

  res = chain.translate("Niakr1s見れば笑って");
  ASSERT_EQ(res.size(), 3);
  ASSERT_EQ(res.chunks()[1]->translations().front()->word(), "見る");
  ASSERT_EQ(res.chunks()[1]->originText(), "見れば");
  ASSERT_EQ(res.chunks()[0]->translations().front()->word(), "Niakr1s");
  ASSERT_EQ(res.chunks()[0]->translations().front()->readings().front(), "Nia");
  ASSERT_EQ(res.chunks()[2]->translations().front()->word(), "笑う");
  ASSERT_EQ(res.chunks()[2]->originText(), "笑って");

  res = chain.translate("見Niakr1sれば笑Niakr1sって");
  ASSERT_EQ(res.size(), 6);
  ASSERT_EQ(res.chunks()[0]->originText(), "見");
  ASSERT_EQ(res.chunks()[1]->originText(), "Niakr1s");
  ASSERT_EQ(res.chunks()[2]->originText(), "れば");
  ASSERT_EQ(res.chunks()[3]->originText(), "笑");
  ASSERT_EQ(res.chunks()[4]->originText(), "Niakr1s");
  ASSERT_EQ(res.chunks()[5]->originText(), "って");
}

#endif  // DICT_CHAINTRANSLATOR_TESTS_H
