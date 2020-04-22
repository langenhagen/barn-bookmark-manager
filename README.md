# Barn Bookmark Manager
A simple, efficient, flexible & standalone website bookmark manager.

- author: Andreas Langenhagen (andreas@langenhagen.cc)

## Design Decisions
Use `X11` for a fast and suckless UI without bloat, decorations and without stylistic elements.  
Use `C strings` rather than `std::strings` since `C strings` are allegedly 20-30 percent faster than
`std::strings`.  
Abbreviate variable names for the sake of readability with conventional abbreviations.  


## TODO
- setup which compiler, linker, etc
- refactor scripts
- adjust .clang-format
- adjust .gitignore
- implement functionality
- meaningful exit codes, maybe from enum
- consider cleanup() function for x11 and stuff prior to exit
- add an optimized Release build
- adjust THIRD_PARTY
- implement gui
- provide example .config folder structure
- watch out for TODOs
- preceed namespace std with scope resolution operator, like ::std
- come up with solution that allows building offline, i.e. provide yaml-cpp version as .tar.gz file
- implement README.md
- promote on reddit
