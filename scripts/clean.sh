#!/bin/bash
# Delete the build folder.
#
# author: andreasl

rm -fr "$(git rev-parse --show-toplevel)/build/";
