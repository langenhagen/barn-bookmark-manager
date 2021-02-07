#!/bin/bash
# Find files C/C++ files with uncommitted changes and check these files with `clang-format`.
#
# Usage:
#   check-clang-format.sh          # check uncommitted files
#   check-clang-format,sh --all    # check all files
#
# author: andreasl

command -v clang-format >/dev/null || { printf 'Error: clang-format required!\n'; exit 1; }

[ "$1" == "--all" ] && check_all=true

if [ "$check_all" == true ]; then
    # run on all C/C++ sources
    found="$(find include src test \( -name "*.h" -or -name "*.hpp" -or -name "*.hxx" \
        -or -name "*.c" -or -name "*.cpp" -or -name "*.cxx" -or -name "*.cc" \))"
else
    # run only on changed/modified C/C++ sources
    found="$(git status --porcelain | grep -E '\.(h|hpp|hxx|c|cpp|cxx|cc)$' \
        | grep -Eo '[^ ].*' | cut -f2- -d' ' | grep -Eo '[^ ].*')"
fi

mapfile -t files <<< "$found"

n_unformatted_files=0
for file in "${files[@]}"; do
    diff=$(clang-format "$file" | diff -u "$file" -)
    if [ -n "$diff" ]; then
        printf -- "${diff}\n"
        (( n_unformatted_files += 1 ))
    fi
done

if [ "$n_unformatted_files" -gt 0 ]; then
    printf '%d files need style fixes\n' "$n_unformatted_files"
else
    if [ "$check_all" == true ]; then
        printf 'All files are ok\n'
    else
        printf 'Changed files are ok\n'
    fi
fi

exit "$n_unformatted_files"
