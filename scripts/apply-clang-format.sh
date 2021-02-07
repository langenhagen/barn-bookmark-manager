#!/bin/bash
# Find files C/C++ files with uncommitted changes and format these files with `clang-format`.
#
# Usage:
#   apply-clang-format.sh          # format uncommitted files
#   apply-clang-format,sh --all    # format all files
#
# author: andreasl

command -v clang-format >/dev/null || { printf 'Error: clang-format required!\n'; exit 1; }

if [ "$1" == "--all" ]; then
    # run on all C/C++ sources
    found="$(find include src test \( -name "*.h" -or -name "*.hpp" -or -name "*.hxx" \
        -or -name "*.c" -or -name "*.cpp" -or -name "*.cxx" -or -name "*.cc" \))"
else
    # run only on changed/modified C/C++ sources
    found="$(git status --porcelain | grep -E '\.(h|hpp|hxx|c|cpp|cxx|cc)$' \
        | grep -Eo '[^ ].*' | cut -f2- -d' ' | grep -Eo '[^ ].*')"
fi

mapfile -t files <<< "$found"
for file in "${files[@]}"; do
    [ -e "$file" ] && clang-format -i "$file"
done
