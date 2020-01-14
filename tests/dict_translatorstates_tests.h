#ifndef DICT_TRANSLATORSTATES_TESTS_H
#define DICT_TRANSLATORSTATES_TESTS_H

#include <gtest/gtest.h>

#include <filesystem>

#include "translator.h"

using namespace dict;

TEST(translator_states, test1) {
  std::filesystem::remove("data/states.json");

  ChainTranslator chain;
  chain.addTranslator(new UserTranslator("data/user_dictionary.txt"));
  chain.addTranslator(new DeinflectTranslator("data/deinflect.json"));
  chain.addTranslator(new YomiTranslator("data"));
  chain.setTranslatorsState(
      std::make_shared<TranslatorsState>("data/states.json"));
  chain.translate("asdf");
}

#endif  // DICT_TRANSLATORSTATES_TESTS_H
