#!/bin/bash
# If the build folder is missing, call cmake
#
# author: andreasl

build_dir="$(git rev-parse --show-toplevel)/build"

if [ ! -e "$build_dir" ]; then
    mkdir -p "$build_dir"
    cd "$build_dir"
    cmake ..
fi

cmake --build "$build_dir"
