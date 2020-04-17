#!/bin/bash
# If the build folder is missing, create it.
# Then run cmake configure and build the application.
#
# author: andreasl

set -e

build_dir="$(git rev-parse --show-toplevel)/build/"

[ -e "$build_dir" ] || mkdir -p "$build_dir"
cd "$build_dir"

cmake .. -GNinja
cmake --build "$build_dir"
