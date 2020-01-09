#ifndef DICT_TRANSLATIONRESULT_TESTS_H
#define DICT_TRANSLATIONRESULT_TESTS_H

#include <gtest/gtest.h>

#include "translationresult.h"
#include "translator.h"

using namespace dict;

TEST(translationresult, test1) {
  auto de = new DeinflectTranslator("data/deinflect.json", nullptr);
  auto deinflected = de->translate("見れば笑って", true);
  //  ASSERT_EQ(deinflected.chunks().size(), 4);
  //  auto deinflected_translated_text = deinflected.translated_texts();
  //  ASSERT_EQ(deinflected_translated_text.size(), 3);
  //  auto yomi = new YomiTranslator("data");
  //  auto translated = yomi->translate("見る笑う", true);
  //  ASSERT_EQ(translated.chunks().size(), 2);

  //  for (auto &t : deinflected_translated_text) {
  //    TranslationResult m = t.mergeWith(translated);
  //    ASSERT_EQ(m.chunks().size(), 2);
  //    ASSERT_EQ(m.chunks()[0]->text(), "見れば");
  //    ASSERT_EQ(m.chunks()[1]->text(), "笑って");
  //  }
}

TEST(translationresult, constructor1) {
  TranslationResult t_r("見れば笑って");
  ASSERT_EQ(t_r.chunks().size(), 6);
}

#endif  // DICT_TRANSLATIONRESULT_TESTS_H
