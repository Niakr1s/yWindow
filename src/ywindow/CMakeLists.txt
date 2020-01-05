set(ywindow_src
    extension.h
    extension.cpp
    ywindow.h
    ywindow.cpp
    display.h
    display.cpp
    status.h
    status.cpp
    textmodel.h
    textmodel.cpp
    textview.h
    textview.cpp
    textcontroller.h
    textcontroller.cpp
    yomistyletextmodel.h
    yomistyletextmodel.cpp
)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

add_library(ywindow SHARED ${ywindow_src})

target_link_libraries(ywindow
    ywindow_lib
    Qt5::Widgets
)
