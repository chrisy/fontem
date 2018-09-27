C Font Embedder
===============

[![Build Status](https://travis-ci.org/chrisy/fontem.svg?branch=master)](https://travis-ci.org/chrisy/fontem)

Overview
--------

A simple C program to turn selected characters from a TrueType font file into
a .c/.h file pair such that the type can be embedded into the binary of a
program. This is particularly useful for Embedded systems that may not have a
file storage device.


Command line parameters
-----------------------

```
    Usage: fontem [OPTION...]
      -f, --font=file        Font filename (default: null)
      -s, --size=integer     Font size (default: 10)
      -c, --chars=string     List of characters to produce (default: "!@#$%^&*()_+-={}|[]\:";'<>?,./`~ ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789")
      -n, --name=file        Output name (without extension) (default: null)
      -d, --dir=dir          Output directory (default: ".")
      --rle                  Use RLE compression

    Help options:
      -?, --help             Show this help message
          --usage            Display brief usage message
```


Example usage
-------------

```bash
   mkdir -p fonts

   ./fontem --font=/usr/share/fonts/truetype/ubuntu-font-family/UbuntuMono-B.ttf \
            --size=10 --name=ubuntumono --dir=fonts
```

This will generate two files, `fonts/font-ubuntumono-10.c` and
`fonts/font-ubuntumono-10.h`.

To use them, you will need to copy `src/resource/fontem.h` to the `fonts`
directory. Examples of these files may be found in the `examples` directory of
this repository.

The `.c` file contains descriptions of each character (glyph) that was
translated. At the end there is a lookup table mapping the glyphs to their
ASCII code. (Note that this table is indexed on the ordinal value of each
character; if you include high-valued UNICODE characters then this will make
for a large empty table; this shortcoming will be addressed in a future
revision.)


Dependencies
------------

Along side GCC and GNU Autotools, this project depends on the `libpopt` and
`libfreetype` libraries.

On Ubuntu the build dependencies can be installed with:

```bash
   sudo apt install build-essential automake autoconf libtool \
                    libpopt-dev libfreetype6-dev
```

And the runtime dependencies, if you need to move the program to another host,
can be installed with:

```bash
   sudo apt install libpopt0 libfreetype6
```


License
-------

This file is distributed under the terms of the MIT License. See the LICENSE
file at the top of this tree, or if it is missing a copy can be found at
http://opensource.org/licenses/MIT
