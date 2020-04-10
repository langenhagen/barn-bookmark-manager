cmake_minimum_required(VERSION 3.10)

set(target "barn-bookmark-manager")

set(VERSION_MAJOR 0)
set(VERSION_MINOR 1)
set(VERSION_PATCH 0)
set(VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH})

project("${target}" VERSION ${VERSION})

# set(CMAKE_CXX_STANDARD 20) # TODO make possible
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

execute_process(
    COMMAND git describe --tags
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    OUTPUT_VARIABLE BBM_VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE)
string(TIMESTAMP BBM_BUILD_TIMESTAMP "%Y-%m-%d-%H-%M" UTC)

add_executable("${target}" "src/main.cpp")

#set(THREADS_PREFER_PTHREAD_FLAG ON)
#find_package(Threads REQUIRED)
find_package(X11 REQUIRED)

# necessary for X11/Xft/Xft.h to find ft2build.h
set(FREETTYPE2_INCLUDE_DIR "/usr/include/freetype2")
target_include_directories("${target}"
    PRIVATE
        "${X11_INCLUDE_DIR}"
        "${FREETTYPE2_INCLUDE_DIR}"
        "include")

target_sources("${target}"
    PRIVATE
        "${CMAKE_CURRENT_BINARY_DIR}/version_info.cpp")

configure_file("templates/version_info.cpp.in" "version_info.cpp")

target_link_libraries("${target}"
    "${X11_LIBRARIES}"
    "Xft")
