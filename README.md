# nc-help

Utility library for ncurses key binding help windows.

## Usage

This library provides utilities to create very specific help windows that display what key bindings do.

Example window content (taken from [spaceinfo](https://github.com/JaMo42/spaceinfo)):

```
+-------------------------------------------------+
|k/↑          Move cursor up                      |
|j/↓          Move cursor down                    |
|K/PgUp       Move cursor up multiple items       |
|J/PgDn       Move cursor down multiple items     |
|g/Home       Move cursor to the start            |
|G/End        Move cursor to the bottom           |
|Enter/Space  Enter the directory under the cursor|
|r/i          Reverse sorting order               |
|'/'          Begin search                        |
|n            Select the next search result       |
|N            Select the previous search result   |
|c            Clear search                        |
|h            Go to a specific path               |
|R            Reload the current directory        |
+-------------------------------------------------+
```

Each entry consists of a key and description pair.
If the content is too wide for the window it gets wrapped.
If the content is too high for the window, the library provides utilities for scrolling the text.

Note that the window border is not drawn by the library, which just lets you specify a border (1 on each side by default) in which you can draw a border.

The display/input loop has to be done by the user.

See the `test.c` and `test.cc` files for C/C++ implementation examples.

## License

nc-help is free and unencumbered public domain software. For more information, see http://unlicense.org/.

