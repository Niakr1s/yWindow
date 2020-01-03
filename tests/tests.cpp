#include <gtest/gtest.h>

#include "dict_loader_tests.h"
#include "dict_query_tests.h"
#include "dict_translator_tests.h"

int main(int argc, char* argv[]) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
