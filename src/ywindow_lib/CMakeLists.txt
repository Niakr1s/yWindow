set(ywindow_lib_src 
    dictionary.h
    dictionary.cpp
    card.h
    card.cpp
    parser.h
    parser.cpp
    loader.h
    loader.cpp
    translator.h
    translator.cpp
    stringconvert.h
    stringconvert.cpp
)

add_library(ywindow_lib STATIC ${ywindow_lib_src})

target_include_directories(ywindow_lib
    PRIVATE
    ${CMAKE_SOURCE_DIR}/3rdparty/jsoncpp/include
    ${Boost_INCLUDE_DIRS}
    PUBLIC
    .
)

target_link_libraries(ywindow_lib
    jsoncpp_lib
)
