## Description

Performs sorting of huge **binary** 64bit files in external memory.
Tested on files around 10 GB, probably will do on files around 100 GB.

## Build

To build, you need SCons (http://www.scons.org/).
Run

`scons`

in the root of repository.

## Run

You can see help running

`./ext_sort --help`

To sort a binary file `in` into file `out`, simply run

`./ext_sort in out`

To convert text file to binary file (in tests only), use

`./text2bin in_text in`

And the opposite (tests only)

`./bin2text out out_text`

## Algorithm

Uses K-Merge sort in external memory (https://en.wikipedia.org/wiki/External_sorting#External_merge_sort).
