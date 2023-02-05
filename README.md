# hxc

A simple hex viewer that allows you to display the contents of a file

## Installation

1. Ensure that you have a C compiler (preferably [gcc](https://gcc.gnu.org/), C99 or later)
2. Run `make`
3. Add to path (optional)

## Usage

```
Usage: hxc [OPTIONS] ... [FILE]

With no FILE, or when FILE is -, read standard input.

Options:
  --columns N                        Display N columns
  --group N                          Groups N bytes
  --splice START:END                 Displays START to END bytes

  -d, --decode                       Decode hex to characters
  -s, --show-offset                  Display offset

  -h, --help                         Display this information
  -v, --version                      Display the version information
```
