# GREP
Grep is command line utility for recognizing and taking action on lines in a file
that match a given regular expression.

## INSTALLATION
### BUILDING FROM SOURCE
All libraries used to build Grep are housed in this repository so only a clone of this repo will suffice.

The following tools are required:
 - cmake 3.0+
 - cmake supported generator i.e make, ninja etc
 - C compiler supported by cmake

```bash
$ git clone https://github.com/bumbuna/GREP.git
$ cd GREP
$ cmake -B build
$ cmake --build build
```
## USAGE
```bash
$ cd <path_to _cmake_build_dir>
$ ./grep [options] <regular_expression> <file>
```
The default action of Grep is to print all matching lines to the standart output.

## CONTRIBUTING
Pull requests are welcome. for major changes, please open an issue first to discuss what you would like to change.

Please make sure to update test as appropriate.

