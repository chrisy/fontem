Font Embedder
=============

Overview
--------

A simple C program to turn selected characters in a TrueType font file
into a .c/.h pair.

Command line parameters
-----------------------

    Usage: fontem [OPTION...]
      -f, --font=file        Font filename (default: null)
      -s, --size=integer     Font size (default: 10)
      -c, --chars=string     List of characters to produce (default: "!@#$%^&*()_+-={}|[]\:";'<>?,./`~ ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789")
      -n, --name=file        Output name (without extension) (default: null)
      -d, --dir=dir          Output directory (default: ".")

    Help options:
      -?, --help             Show this help message
          --usage            Display brief usage message

Example usage
-------------

    mkdir -p fonts

    ./fontem --font=/usr/share/fonts/truetype/ubuntu-font-family/UbuntuMono-B.ttf \
        --size=10 --name=ubuntumono --dir=fonts

This will generate two files, `fonts/font-ubuntumono-10.c` and
`fonts/font-ubuntumono-10.h`. To use them, you will need to copy
`src/resource/fontem.h` to the `fonts` directory. Examples of
these files may be found in the `examples` directory of this
repository.

The `.c` file contains descriptions of each character (glyph)
that was translated. At the end there is a lookup table mapping
the glyphs to their ASCII code. (Note that this table is indexed
on the ordinal value of each character; if you include high-valued
UNICODE characters then this will make for a large empty table;
this shortcoming will be addressed in a future revision.)

License
-------

This file is distributed under the terms of the MIT License.
See the LICENSE file at the top of this tree, or if it is missing a copy can
be found at http://opensource.org/licenses/MIT

