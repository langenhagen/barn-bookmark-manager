cmake_minimum_required(VERSION 3.10)

set(target "barn-bookmark-manager")
project("${target}" VERSION 0.1.0)
# set(CMAKE_CXX_STANDARD 20) # TODO make possible
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

execute_process(
    COMMAND git describe
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    OUTPUT_VARIABLE BBM_VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE)
string(TIMESTAMP BBM_BUILD_TIMESTAMP "%Y-%m-%d-%H-%M" UTC)

add_executable("${target}" "src/main.cpp")

target_include_directories("${target}"
    PRIVATE
        "include")


configure_file("templates/version_info.cpp.in" "version_info.cpp")

target_sources("${target}"
    PRIVATE
        "${CMAKE_CURRENT_BINARY_DIR}/version_info.cpp")
