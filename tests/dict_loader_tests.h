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
  Loader* loader = Loader::getFilesystemLoader("data/jmdict_english");
  Dictionary* dictionary = new YomiDictionary();
  loader->loadInto(dictionary);
  ASSERT_EQ(dictionary->info(), "JMdict (English)");
  ASSERT_EQ(dynamic_cast<YomiDictionary*>(dictionary)->tags().size(), 4);
  ASSERT_EQ(dictionary->cards().size(), 6);
}

TEST(fs_dict_loader, term_async) {
  Loader* loader = Loader::getFilesystemLoader("data/jmdict_english");
  Dictionary* dictionary = new YomiDictionary();
  auto f = loader->loadIntoAsync(dictionary);
  f.wait();
  ASSERT_EQ(dictionary->info(), "JMdict (English)");
  ASSERT_EQ(dynamic_cast<YomiDictionary*>(dictionary)->tags().size(), 4);
  ASSERT_EQ(dictionary->cards().size(), 6);
}

TEST(fs_dict_loader, kanji) {
  auto loader = std::unique_ptr<Loader>(
      Loader::getFilesystemLoader("data/kanjidic_english"));
  Dictionary* dictionary = new YomiDictionary();
  loader->loadInto(dictionary);
  ASSERT_EQ(dictionary->info(), "KANJIDIC (English)");
  ASSERT_EQ(dynamic_cast<YomiDictionary*>(dictionary)->tags().size(), 3);
  ASSERT_EQ(dictionary->cards().size(), 3);
}

TEST(fs_dict_loader, empty) {
  auto loader =
      std::unique_ptr<Loader>(Loader::getFilesystemLoader("data/empty_dir"));
  Dictionary* dictionary = new YomiDictionary();
  loader->loadInto(dictionary);
  ASSERT_EQ(dictionary->info(), "");
  ASSERT_EQ(dynamic_cast<YomiDictionary*>(dictionary)->tags().size(), 0);
  ASSERT_EQ(dictionary->cards().size(), 0);
}

TEST(fs_dict_loader, non_existent_dir) {
  ASSERT_THROW(Loader::getFilesystemLoader("data/non_existent_dir"),
               std::invalid_argument);
}

#endif  // DICT_LOADER_TESTS_H
