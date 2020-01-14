#include <gtest/gtest.h>

#include "Windows.h"
#include "dict_chaintranslator_tests.h"
#include "dict_loader_tests.h"
#include "dict_query_tests.h"
#include "dict_translationresult_tests.h"
#include "dict_translator_tests.h"
#include "dict_translatorstates_tests.h"

int main(int argc, char* argv[]) {
  SetConsoleOutputCP(CP_UTF8);
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
