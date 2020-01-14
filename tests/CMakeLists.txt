cmake_minimum_required(VERSION 2.8)

enable_testing()
find_package(GTest REQUIRED)

add_executable(tests
    tests.cpp
    dict_loader_tests.h
    dict_query_tests.h
    dict_translator_tests.h
    dict_translationresult_tests.h
    dict_chaintranslator_tests.h
)

file(COPY data DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

target_link_libraries(tests
    GTest::GTest GTest::Main
    ywindow_lib
)

add_test(AllTestsInFoo tests)
