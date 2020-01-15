set(ywindow_src
    extension.h
    extension.cpp
    ywindow.h
    ywindow.cpp
    status.h
    status.cpp
    textmodel.h
    textmodel.cpp
    textview.h
    textview.cpp
    translationview.h
    translationview.cpp
    textcontroller.h
    textcontroller.cpp
    hoversyntaxhighlighter.h
    hoversyntaxhighlighter.cpp
    translationconverter.h
    translationconverter.cpp
    NLTemplate.h
    NLTemplate.cpp
    texttohtml.h
    texttohtml.cpp
    settings.h
    settings.cpp
    settingswindow.h
    settingswindow.cpp
    translatorssettingsview.h
    translatorssettingsview.cpp
)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

add_library(ywindow SHARED ${ywindow_src})

target_link_libraries(ywindow
    ywindow_lib
    Qt5::Widgets
)

target_include_directories(ywindow
    PRIVATE
    ${Boost_INCLUDE_DIRS}
)

