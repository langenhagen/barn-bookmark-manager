# Barn Bookmark Manager
A simple, efficient, flexible & standalone website bookmark manager.

- author: Andreas Langenhagen (andreas@langenhagen.cc)


## Installation
You can build and run this project directly on your system.
See detailed instructions below.

### Prerequisites & Dependencies
If you choose to compile and run Barn's Bookmark Manager directly on your system, please note
that the following packages must be installed on your system:

- the C++ 17 standard library from the compiler (tested with `GNU` `7.5.0.`)
- CMake
- the curl development libraries (`libcurl4-openssl-dev` on `Ubuntu`)
- the xml c parser libraries and toolkit (`libxml2` on `Ubuntu`)
- the X11 development libraries (`libx11-dev` on `Ubuntu`)
- the Xft development libraries (`libxft-dev` on `Ubuntu`)
- the xdo development libraries (`libxdo-dev` on `Ubuntu`)
- the application xclip (`xclip` on `Ubuntu`)

Barn's Bookmark Manager also downloads source code of 3rd-party libraries during the CMake configure
step:

- [yaml-cpp](https://github.com/jbeder/yaml-cpp/) (Copyright 2008-2015, Jesse Beder)

To install the requirements on Ubuntu, refer to the script:
`scripts/install-requirements-ubuntu.sh`.


## Building and Running
Invoke the following script to build the entire project:

```bash
./scripts/configure-and-build.sh
```

To run the executables, call:

```bash
./build/bbm-add
./build/bbm-fetch
```


## Contributing
Work on your stuff locally, branch, commit and modify to your heart's content.
At the moment, there is no origin for this git repository!
Happy coding!


## License
See [LICENSE](LICENSE) file.


## Design Decisions
Use `X11` for a fast and suckless UI without bloat, decorations and without stylistic elements.  

Use for the X11 GUI part, use `C strings` rather than `std::strings` since `C strings` are allegedly
20-30 percent faster than `std::strings`.  
Abbreviate variable names for the sake of readability with conventional abbreviations.  

Allow every string to be accepted an address in order to avoid expensive validity checks and
maximize flexibility.  


## TODO
- handle missing settings nicely
- use $EDITOR instead of vim in settings, with vim as fallback
- handle compile time options nicely
- remove dependency libxdo, possibly with help of https://gist.github.com/kui/2622504
- adjust .clang-format
- adjust .gitignore
- implement functionality
- consider cleanup() function for x11 and stuff prior to exit
- add an optimized Release build
- review THIRD_PARTY
- add usage section
- implement gui
- provide example .config folder structure
- watch out for TODOs
- preceed namespace std with scope resolution operator, like ::std
- come up with solution that allows building offline, i.e. provide yaml-cpp version as .tar.gz file
- implement README.md
- write blog post
- promote on reddit
