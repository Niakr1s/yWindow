add_library(ywindow_lib "")

target_include_directories(ywindow_lib PRIVATE
    ../3rdparty/jsoncpp/include
    ${Boost_INCLUDE_DIRS}
)
target_link_libraries(ywindow_lib
    jsoncpp_lib_static
)

add_subdirectory(dict)
