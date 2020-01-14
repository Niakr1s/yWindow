#ifndef DICT_TRANSLATORSTATES_TESTS_H
#define DICT_TRANSLATORSTATES_TESTS_H

#include <gtest/gtest.h>
#include <json/json.h>

#include <filesystem>

#include "translator.h"

using namespace dict;

namespace fs = std::filesystem;

TEST(translator_states, test1) {
  const fs::path json_path("data/empty_states.json");
  fs::remove(json_path);

  ChainTranslator chain;
  chain.addTranslator(new UserTranslator("data/user_dictionary.txt"));
  chain.addTranslator(new DeinflectTranslator("data/deinflect.json"));
  chain.addTranslator(new YomiTranslator("data"));
  chain.setTranslatorsState(std::make_shared<TranslatorsState>(json_path));
  chain.translate("asdf");

  Json::Value root;
  std::ifstream is(json_path);
  is >> root;

  ASSERT_EQ(root.size(), 3);
  ASSERT_EQ(root[0]["translator_info"].asString(), "DeinflectTranslator");
  ASSERT_EQ(root[0]["unordered"].size(), 1);
  ASSERT_TRUE(root[0]["ordered"].empty());
  ASSERT_TRUE(root[0]["disabled"].empty());
  ASSERT_EQ(root[1]["translator_info"].asString(), "UserTranslator");
  ASSERT_EQ(root[1]["unordered"].size(), 1);
  ASSERT_TRUE(root[1]["ordered"].empty());
  ASSERT_TRUE(root[1]["disabled"].empty());
  ASSERT_EQ(root[2]["translator_info"].asString(), "YomiTranslator");
  ASSERT_EQ(root[2]["unordered"].size(), 2);
  ASSERT_TRUE(root[2]["ordered"].empty());
  ASSERT_TRUE(root[2]["disabled"].empty());
}

#endif  // DICT_TRANSLATORSTATES_TESTS_H
