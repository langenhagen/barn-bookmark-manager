#!/bin/bash
# Delete the build folder.
#
# author: andreasl

cd "$(git rev-parse --show-toplevel)"
rm -fr "build/";
