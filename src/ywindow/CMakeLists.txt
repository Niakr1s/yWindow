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
    ystyler.h
    ystyler.cpp
    hoversyntaxhighlighter.h
    hoversyntaxhighlighter.cpp
    translationtohtmlconverter.h
    translationtohtmlconverter.cpp
    NLTemplate.h
    NLTemplate.cpp
)

file(COPY templates DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
configure_file(templates/ytranslation.html
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/templates/ytranslation.html COPYONLY)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

add_library(ywindow SHARED ${ywindow_src})

target_link_libraries(ywindow
    ywindow_lib
    Qt5::Widgets
)


