# CMakeList.txt : CMake project for yWindow, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.13)

if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
      CACHE STRING "")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if (MSVC)
    set(CMAKE_CXX_FLAGS "/EHsc")
elseif (MINGW)
    set(CMAKE_CXX_FLAGS "-mwindows")
endif ()

set(BUILD_SHARED_LIBS ON CACHE STRING "")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/a")

add_definitions( -DBOOST_ALL_DYN_LINK )

project (yWindow LANGUAGES CXX VERSION 0.1.0)

find_package(Boost)
find_package(jsoncpp CONFIG REQUIRED)

add_subdirectory(src)

option(WITH_TESTS OFF)
if (WITH_TESTS)
    add_subdirectory(tests)
endif ()

file(COPY data/yWindow DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

configure_file(src/version.h.in ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/version.h)
