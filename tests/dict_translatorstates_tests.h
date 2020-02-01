#ifndef DICT_TRANSLATORSTATES_TESTS_H
#define DICT_TRANSLATORSTATES_TESTS_H

#include <gtest/gtest.h>
#include <json/json.h>

#include <filesystem>

#include "translator.h"

using namespace dict;

namespace fs = std::filesystem;

TEST(translator_states, test2) {
  const fs::path json_path("data/states_copy.json");
  fs::remove(json_path);
  fs::copy_file("data/states.json", json_path);

  ChainTranslator chain;
  chain.addTranslator(new UserTranslator("data/user"));
  chain.addTranslator(new DeinflectTranslator("data/deinflect"));
  chain.addTranslator(new YomiTranslator("data/yomi"));
  auto translators_settings = std::make_shared<TranslatorsSettings>(json_path);
  chain.setTranslatorsSettings(translators_settings);

  Json::Value root;
  std::ifstream is(json_path);
  is >> root;

  ASSERT_EQ(root.size(), 3);
  ASSERT_EQ(root[0]["translator_info"].asString(), "DeinflectTranslator");
  ASSERT_EQ(root[0]["dictionaries"].size(), 1);
  ASSERT_TRUE(root[0]["dictionaries"][0]["enabled"].asBool());

  ASSERT_EQ(root[1]["translator_info"].asString(), "UserTranslator");
  ASSERT_EQ(root[1]["dictionaries"].size(), 1);
  ASSERT_TRUE(root[1]["dictionaries"][0]["enabled"].asBool());

  ASSERT_EQ(root[2]["translator_info"].asString(), "YomiTranslator");
  ASSERT_EQ(root[2]["dictionaries"].size(), 2);
  ASSERT_FALSE(root[2]["dictionaries"][0]["enabled"].asBool());
  ASSERT_FALSE(root[2]["dictionaries"][1]["enabled"].asBool());
}

TEST(translator_states, test1) {
  const fs::path json_path("data/empty_states.json");
  fs::remove(json_path);

  ChainTranslator chain;
  chain.addTranslator(new UserTranslator("data/user"));
  chain.addTranslator(new DeinflectTranslator("data/deinflect"));
  chain.addTranslator(new YomiTranslator("data/yomi"));
  chain.setTranslatorsSettings(
      std::make_shared<TranslatorsSettings>(json_path));

  Json::Value root;
  std::ifstream is(json_path);
  is >> root;

  ASSERT_EQ(root.size(), 3);
  ASSERT_EQ(root[0]["translator_info"].asString(), "DeinflectTranslator");
  ASSERT_EQ(root[0]["dictionaries"].size(), 1);
  ASSERT_TRUE(root[0]["dictionaries"][0]["enabled"].asBool());

  ASSERT_EQ(root[1]["translator_info"].asString(), "UserTranslator");
  ASSERT_EQ(root[1]["dictionaries"].size(), 1);
  ASSERT_TRUE(root[1]["dictionaries"][0]["enabled"].asBool());

  ASSERT_EQ(root[2]["translator_info"].asString(), "YomiTranslator");
  ASSERT_EQ(root[2]["dictionaries"].size(), 2);
}

#endif  // DICT_TRANSLATORSTATES_TESTS_H
