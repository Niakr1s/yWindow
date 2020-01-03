#ifndef DICT_LOADER_TESTS_H
#define DICT_LOADER_TESTS_H

#include <gtest/gtest.h>

#include <filesystem>
#include <memory>

#include "dictionary.h"
#include "loader.h"

namespace fs = std::filesystem;

using namespace dict;

TEST(fs_dict_loader, term) {
  auto future =
      Loader::loadFromDirectory<YomiDictionary>("data/jmdict_english");
  auto dictionary = dynamic_cast<YomiDictionary*>(future.get());
  ASSERT_EQ(dictionary->info(), "JMdict (English)");
  ASSERT_EQ(dictionary->tags().size(), 4);
  ASSERT_EQ(dictionary->cards().size(), 6);
}

TEST(fs_dict_loader, kanji) {
  auto future =
      Loader::loadFromDirectory<YomiDictionary>("data/kanjidic_english");
  auto dictionary = dynamic_cast<YomiDictionary*>(future.get());
  ASSERT_EQ(dictionary->info(), "KANJIDIC (English)");
  ASSERT_EQ(dictionary->tags().size(), 3);
  ASSERT_EQ(dictionary->cards().size(), 3);
}

TEST(fs_dict_loader, empty) {
  auto future = Loader::loadFromDirectory<YomiDictionary>("data/empty_dir");
  auto dictionary = dynamic_cast<YomiDictionary*>(future.get());
  ASSERT_EQ(dictionary->info(), "");
  ASSERT_EQ(dictionary->tags().size(), 0);
  ASSERT_EQ(dictionary->cards().size(), 0);
}

TEST(fs_dict_loader, non_existent_dir) {
  ASSERT_THROW(
      Loader::loadFromDirectory<YomiDictionary>("data/non_existent_dir").get(),
      std::invalid_argument);
}

#endif  // DICT_LOADER_TESTS_H
