# Barn's Bookmark Manager
Thoughts on a potential self-implemented bookmark manager.


## Steps
Next step:

1. Add bookmark app
    - settings / dynamicity
2. show bookmark app


## Overall Traits
The bookmark app is a separate application. The bookmarks app is not integrated into your browser.
The desktop app is completely controllable via keyboard.
Have preferably "suckless" look and feel. Especially speed and style. Make the app react instantly.
Use the filesystem, no database bullshit.

Create folders à la `mkdir -p` prior to access since it could be that they are not (yet) reflected
in the file system.

Leave maintenance of folder structure to the user, i.e. leave empty folders untouched.


## Add Bookmarks
- start via keyboard shortcut
- take the link from the clipboard or the invocation command
- ask for path with fuzzy suggestions and 5 recent history choices
- [ask for tags with fuzzy suggestions and 5 recent history choices]
- [ask for markdown comment]
- [ask for 1-5 star rating (keys: `1`, `2`, `3`, `4`, `5`, `<ENTER>` means no rating)]
- create bookmark id `<id>`
- add bookmark id `<id>` to file `ids` as most recent entry, i.e. add it to the end
- store the bookmark info into `<id>.yaml` and download the website's text to `<id>.site.txt`
- `ctrl+<ENTER>` or `<ENTER>` is Ok/Continue, `<ESC>` is Abort


## Query Bookmarks
- load entries from `folders`
- load entries from `ids`
- remove entries from `folders` array that don't exist on the file system
- add entries to `folders` array that are not in it and make them most recent, i.e. add them to the
  end
- remove entries from `ids` array for bookmarks that are missing on file system and give a warning
- add entries to `ids` array for bookmarks that are missing from `ids` and give a warning
- search with fuzzy search and include `tags`, `*.site.txt` files, bookmark paths, bookmark urls,
  names, recency
- show list view of matches browseable with up-down arrow keys keys
- show preview of the currently selected bookmark with url, title, comment, rating, and excerpt of
  textual copy from `site.txt`
- write updated array values to `folders`
- write updated array values to `ids`


## Maintain Bookmarks
- to edit a bookmark, query a bookmark and select edit (press `e`).
- drop into the bookmark's `<id>.yaml` file with your set text editor.

- to move a bookmark, query a bookmark and select move (press `m`).
- drop into a file explorer with the current bookmark folder open
- make according changes in the file `ids`, i.e. make the moved bookmark most recent


## Remove Bookmarks
- query a bookmark and select delete (key: `d`)
- remove the bookmark from the file `ids`
- delete the bookmark files `<id>.{yaml,site.txt}`
- keep possibly empty filesystem folders and entries in `folders` and `tags`, i.e. don't touch them


## Bookmark Storage File System Structure
Store the bookmarks in a filesystem as opposed to a database.
A `mysql` or `mongodb` database may make sense when implementing mobile apps.
```
~
└── .config/
    └── barn-bookmarks/                         # barn-bookmarks root folder
        ├── folders                             # paths to bookmarks
        ├── ids
        ├── settings.yaml
        ├── tags
        └── bookmarks/
            ├── some/
            │   ├── 1a2b3c.yaml
            │   ├── 1a2b3c.site.txt
            │   ├── 2b3c4d.yaml
            │   ├── 2b3c4d.site.txt
            │   └── folder/
            │       └── structure/
            │           ├── 3c4d5e.yaml
            │           └── 3c4d5e.site.txt
            ├── emptyfolder/
            ├── 4d5e6f.yaml
            └── 4d5e6f.site.txt
```


## File Structures

### settings.yaml
See implementation.

### ids
Store all ids of all available bookmark folders sorted by recency from old to most recent.
Recency entails adding, modifying and fetching.
Every id should be unique.

Ids should be short alphanumeric strings. 6 characters should suffice to reliably store all data.

Example:
```
some/1a2b3c
some/2b3c4d
some/folder/structure/3c4d5e
4x5y6z
```

### folders
Store all available folders, sorted by recency from old to most recent.
Every folder is unique.
Example:
```
some/
some/deleted/folder/
some/folder/
```

### tags
Store all available tags sorted by recency from old to most recent.
Every tag is unique.
Example:
```
cpp
talk
mgmt
py
```

### <id>.{yaml,site.txt}
Store every info to a certain bookmark in such files.

#### <id>.yaml
Store all bookmark info in `<id>.yaml`.
```yaml
title: A Nice Blog Post!
url: https://andiweb.com/2020/02/28/a-nice-blogpost#even-nicer
created: 2020-02-29 16:56
last_access: 2020-02-29 17:02
tags: [recommend, cmake]
rating: 5
comment: |
  This is a comment to an article.
  The article was very nice and might help in doing x.
```

#### <id>.site.txt
Store the scraped text of the bookmarked web page in this file for later full text search.
```
A Nice Blog Post!

Lorem Ipsum dolor ...
```


## Chrome Browser Extensions
- add bookmark for current site via keyboard shortcut
    - find out if extensions can interface with external applications & file system
    - find out if keyboard to invoke extension are possible


## Synchronization
Sync Bookmarks across devices without further work. Use Nextcloud to sync the bookmarks.

Nextcloud scales poorly, but sharing bookmarks basically only with myself, I suspect that may be
fine.

Alternatively consider Dropbox or Google Drive or Syncthing for self-maintained syncing.
Also consider a self-hosted server. However, a special server implies a daemon on client side that
syncs regularly and after adding/removing bookmarks.


## Webserver
Have a static, password-protected, ssl-encrypted web interface for the bookmark manager.
Read-only info may be sufficient at first


## Mobile
Cover mobile use case first with a website. Later, consider a dedicated app.


## Prominent Operations of the Bookmark manager program
- parse `yaml` files
- read & write files & folders
- iterate the file system
- create & delete files & folders
- id function of a string
- present a UI with focus
- listen to keyboard input
- invoke external commands with how-to-call-an-external-program-with-parameters


## Programming Language
Use C++ as the programming language.

- Bash lacks sufficient UI, but would be most convenient and very fast due to `grep`, `find` and
  `sed`.

- `Python` provides many UI-ways, and I could start out of the box. However, filesystem operations
  in Python may be very, very slow.
. I found a remedy in https://stackoverflow.com/questions/1724693/find-a-file-in-python:
  ```python
  paths = [line[2:] for line in subprocess.check_output("find . -iname '*.txt'",
  shell=True).splitlines()]
  ```
  However, this makes a Python solution ugly.

- C++ is likely fast and and has many options for UIs. However, I have to make myself comfortable
  with C++ again. Also, I may be tempted to use C++ 20 features just for fun. This could delay the
  result

The function family of `execl()` https://linux.die.net/man/3/execl, or `popen()` may be helpful
See:
https://en.cppreference.com/w/cpp/regex

Finding files in C++ may be done via std::filesystem:
https://stackoverflow.com/questions/612097/how-can-i-get-the-list-of-files-in-a-directory-using-c-or-c

## Getting the URL From the Browser

### Use libxdo
`libxdo` is minimally faster and needs less dependencies after compilation than with a bash-script.
Also, using a compiled library, as opposed to calling a bash script from c++, may feel a bit more by
the book. However, both `libxdo` and a bash-scripts are viable.

#### libxdo vs bash-script

bash-script (-2):
+ flexible / adjustable to any platform, browser w/out compilation
- minimally slower
- needs the according on dev machine i.e. xdotool
- needs xdotool to run

libxdo (-1):
- less flexible / needs compilation
+ minimally faster
- needs libraries for compilation
