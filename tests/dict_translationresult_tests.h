#ifndef DICT_TRANSLATIONRESULT_TESTS_H
#define DICT_TRANSLATIONRESULT_TESTS_H

#include <gtest/gtest.h>

#include "translationresult.h"
#include "translator.h"

using namespace dict;

TEST(translationresult, test1) {
  auto de = new DeinflectTranslator("data/deinflect.json", nullptr);
  auto deinflected = de->translate("見れば笑って", true);
  ASSERT_EQ(deinflected.chunks().size(), 4);
  auto deinflected_translated_text = deinflected.translated_texts();
  ASSERT_EQ(deinflected_translated_text.size(), 3);
  auto yomi = new YomiTranslator("data");
  auto translated = yomi->translate("見る笑う", true);
  ASSERT_EQ(translated.chunks().size(), 2);

  auto merged = translated.merged(deinflected);
}

#endif  // DICT_TRANSLATIONRESULT_TESTS_H
