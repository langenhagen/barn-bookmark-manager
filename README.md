# Barn Bookmark Manager
A simple, efficient, flexible & standalone website bookmark manager.

- author: Andreas Langenhagen (andreas@langenhagen.cc)

## Design Decisions
Use `X11` for a fast and suckless UI without bloat, decorations and without stylistic elements.  
Use `C strings` rather than `std::strings` since `C strings` are allegedly 20-30 percent faster than
`std::strings`.  
Abbreviate variable names for the sake of readability with conventional abbreviations.  


## TODO
- setup project structure
- setup which compiler, linker, etc
- refactor scripts
- adjust .clang-format
- adjust .gitignore
- adjust THIRD_PARTY
- implement gui
- provide example .config folder structure
- implement functionality
- implement README.md
- promote on reddit
