#!/bin/bash
# Build the project and run the application.
#
# author: andreasl

project_dir="$(git rev-parse --show-toplevel)"
if "${project_dir}/scripts/configure-and-build.sh" ; then
    "${project_dir}/build/barn-bookmark-manager"
fi
