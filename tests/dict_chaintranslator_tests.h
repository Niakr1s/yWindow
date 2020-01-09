#ifndef DICT_CHAINTRANSLATOR_TESTS_H
#define DICT_CHAINTRANSLATOR_TESTS_H

#include <gtest/gtest.h>

#include <string>

#include "translationresult.h"
#include "translator.h"

using namespace dict;

TEST(chaintranslator, test1) {
  ChainTranslator chain;
  Dictionary* user = new UserDictionary();
}

#endif  // DICT_CHAINTRANSLATOR_TESTS_H
