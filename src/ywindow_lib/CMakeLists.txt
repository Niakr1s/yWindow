﻿set(ywindow_lib_src 
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
    exceptions.h
    exceptions.cpp
    translationresult.h
    translationresult.cpp
    translatorssettings.h
    translatorssettings.cpp
)

add_library(ywindow_lib STATIC ${ywindow_lib_src})

target_include_directories(ywindow_lib
    PRIVATE
    ${CMAKE_SOURCE_DIR}/3rdparty/utf8cpp/source
    ${Boost_INCLUDE_DIRS}
    PUBLIC
    .
)

target_link_libraries(ywindow_lib
    jsoncpp_lib
)
