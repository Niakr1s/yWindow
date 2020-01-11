#ifndef DICT_TRANSLATIONRESULT_TESTS_H
#define DICT_TRANSLATIONRESULT_TESTS_H

#include <gtest/gtest.h>

#include "translationresult.h"
#include "translator.h"

using namespace dict;

TEST(translationresult, test1) {
  auto de = new DeinflectTranslator("data/deinflect.json", nullptr);
  auto deinflected = de->translate("見れば笑って");
  ASSERT_EQ(deinflected.chunks().size(), 4);
  auto deinflected_translated_text = deinflected.toTexts();
  ASSERT_EQ(deinflected_translated_text.size(), 3);
  auto yomi = new YomiTranslator("data");

  for (auto &t : deinflected_translated_text) {
    auto to_translate = t.string();
    auto translated = yomi->translate(to_translate);
    TranslationResult m = t + translated;
    ASSERT_EQ(m.orig_text(), "見れば笑って");
    ASSERT_EQ(m.chunks()[0]->originText(), "見れば");
    ASSERT_EQ(m.chunks()[0]->translations().front()->name(), "見る");
  }
}

TEST(translationresult, constructor1) {
  TranslationResult t_r("見れば笑って");
  ASSERT_EQ(t_r.chunks().size(), 6);
  ASSERT_EQ(t_r.chunks()[0]->originText(), "見");
  ASSERT_EQ(t_r.chunks()[1]->originText(), "れ");
  ASSERT_EQ(t_r.chunks()[2]->originText(), "ば");
  ASSERT_EQ(t_r.chunks()[3]->originText(), "笑");
  ASSERT_EQ(t_r.chunks()[4]->originText(), "っ");
  ASSERT_EQ(t_r.chunks()[5]->originText(), "て");
}

#endif  // DICT_TRANSLATIONRESULT_TESTS_H
