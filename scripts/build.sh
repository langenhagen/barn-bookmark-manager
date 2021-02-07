#!/bin/bash
# Build the executables.
# Omit the cmake configure step.
#
# author: andreasl
set -e

project_dir="$(git rev-parse --show-toplevel)"

cd "${project_dir}/build/"
cmake --build .
