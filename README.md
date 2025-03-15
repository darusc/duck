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
  -c,  --count                Show item count
  -e,  --exclude [extensions] Exclude files with specified extensions
  -i,  --include [extensions] Include only files with specified extensions
  -h,  --help                 Print this info
  -hf, --hidden               Ignore hidden files.
  -s,  --sort <method>        Sort by (size is default)
        methods: <size|alphabetic|items|>
```

## Performance
Running duck on a **100GB** directory with **900k files and 100k directories** took around **5sec (with a warm cache)** / **20sec (with a cold cache)** and used ~300mb of memory.

Run on Windows 11 24H2 machine with Ryzen 9 7940HS and the drive is located on an SSD. Performance may very according to the machine duck is run on.

### Warm cache
```
Files: 916529  Directories: 109097
Discovery time: 5.025 sec
Total time: 5.025 sec

111.18 GB ~/
 45.11 GB 40.58% [██████         ] Qt/
 28.00 GB 25.19% [████           ] Xilinx/
 13.16 GB 11.84% [██             ] Microsoft Visual Studio/
  4.65 GB  4.18% [█              ] CLion/
  4.56 GB  4.10% [█              ] JetBrains Rider 2024.1.1/
  3.91 GB  3.51% [█              ] intelFPGA/
  3.47 GB  3.12% [               ] IntelliJ IDEA 2024.1.1/
  2.71 GB  2.43% [               ] PyCharm Professional/
  2.58 GB  2.32% [               ] Android/
  1.93 GB  1.73% [               ] DataGrip/
499.33 MB  0.44% [               ] Arduino/
418.72 MB  0.37% [               ] Microsoft VS Code/
148.13 MB  0.13% [               ] Logisim Evolution/
 33.35 MB  0.03% [               ] ollydbg/
 10.10 MB  0.01% [               ] AIMSpice/
  4.48 MB  0.00% [               ] npp/
  4.00 MB  0.00% [               ] masm_minimal/
  2.39 MB  0.00% [               ] nasm-2.16.01/
```

### Cold cache
```
Files: 916529  Directories: 109097
Discovery time: 21.198 sec
Total time: 21.198 sec
111.18 GB ~/
 45.11 GB 40.58% [██████         ] Qt/
 28.00 GB 25.19% [████           ] Xilinx/
 13.16 GB 11.84% [██             ] Microsoft Visual Studio/
  4.65 GB  4.18% [█              ] CLion/
  4.56 GB  4.10% [█              ] JetBrains Rider 2024.1.1/
  3.91 GB  3.51% [█              ] intelFPGA/
  3.47 GB  3.12% [               ] IntelliJ IDEA 2024.1.1/
  2.71 GB  2.43% [               ] PyCharm Professional/
  2.58 GB  2.32% [               ] Android/
  1.93 GB  1.73% [               ] DataGrip/
499.33 MB  0.44% [               ] Arduino/
418.72 MB  0.37% [               ] Microsoft VS Code/
148.13 MB  0.13% [               ] Logisim Evolution/
 33.35 MB  0.03% [               ] ollydbg/
 10.10 MB  0.01% [               ] AIMSpice/
  4.48 MB  0.00% [               ] npp/
  4.00 MB  0.00% [               ] masm_minimal/
  2.39 MB  0.00% [               ] nasm-2.16.01/
```

## Development

External dependencies are kept to a minimum. The windows version uses only the win32 api with no additional dependencies, while the linux version uses [ncurses](https://invisible-island.net/ncurses/) to easily manage the terminal.

To build duck use the provided makefile.

The project is modular (only the terminal ui is platform specifc), `duiw.c` contains the UI implementation for win32 platforms and `duil.c` contains the UI implementation for unix based platforms. 