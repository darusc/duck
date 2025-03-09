     ____  _   _  ____ _  __
    |  _ \| | | |/ ___| |/ /
    | | | | | | | |   | ' / 
    | |_| | |_| | |___| . \ 
    |____/ \___/ \____|_|\_\

    duck - Disk usage analysis tool & file explorer.

## About

DUCK is a cross-platform interactive terminal-based disk analysis tool and file explorer designed for efficiency and ease of use. It provides a visually intuitive way to analyze disk space usage and navigate directories directly from the terminal.

<br>

<img center src="imgs/duck.gif">

## Usage

```
  duck <directory> [options]

  Options:
  -b,  --benchmark            Benchmark execution
  -h,  --help                 Print this info
  -hf, --hidden               Ignore hidden files.
  -s,  --sort <method>        Sort by (size is default)
        methods: <size|alphabetic|items|>
```

## Development

External dependencies are kept to a minimum. The windows version uses only the win32 api with no additional dependencies, while the linux version uses [ncurses](https://invisible-island.net/ncurses/) to easily manage the terminal.

To build duck use the provided makefile.

The project is modular (only the terminal ui is platform specifc), `duiw.c` contains the UI implementation for win32 platforms and `duil.c` contains the UI implementation for unix based platforms. 