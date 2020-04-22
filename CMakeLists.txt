# Main CMakeLists.txt file for Barn's Bookmark Manager.
#
# author: andreasl

cmake_minimum_required(VERSION 3.10)

# version and timestamp
execute_process(
    COMMAND git describe --tags
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    OUTPUT_VARIABLE BBM_VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE)
string(TIMESTAMP BBM_BUILD_TIMESTAMP "%Y-%m-%d-%H-%M" UTC)

project("barn-bookmark-manager" VERSION "${BBM_SEM_VER}")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(experimental_libraries c++experimental)
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(experimental_libraries stdc++fs)
endif()

# TODO revisit when threading becomes a topic
#set(THREADS_PREFER_PTHREAD_FLAG ON)
#find_package(Threads REQUIRED)
find_package(X11 REQUIRED)
find_package(Freetype REQUIRED)
include("cmake/yaml-cpp.cmake")

configure_file("templates/version_info.hpp.in" "include/version_info.hpp")

# Add bookmark app
include("cmake/add-sources.cmake")
add_executable(bbm-add "${BBM_ADD_SOURCES}")

target_include_directories(bbm-add
    PRIVATE
        "${CMAKE_CURRENT_BINARY_DIR}/include"
        "${FREETYPE_INCLUDE_DIRS}"
        "${X11_INCLUDE_PATH}"
        "${YAML_CPP_INCLUDE_PATH}"
        "include")

target_link_libraries(bbm-add
    "${experimental_libraries}"
    "${X11_LIBRARIES}"
    "xdo"
    "Xft"
    "${YAML_CPP_LIB}")

# Fetch bookmark app
include("cmake/fetch-sources.cmake")
add_executable(bbm-fetch "${BBM_FETCH_SOURCES}")

target_include_directories(bbm-fetch
    PRIVATE
        "${CMAKE_CURRENT_BINARY_DIR}/include"
        "${FREETYPE_INCLUDE_DIRS}"
        "${X11_INCLUDE_PATH}"
        "${YAML_CPP_INCLUDE_PATH}"
        "include")

target_link_libraries(bbm-fetch
    "${experimental_libraries}"
    "${X11_LIBRARIES}"
    "Xft"
    "${YAML_CPP_LIB}")
