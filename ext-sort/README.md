## Description

Performs sorting of huge **binary** files in external memory.
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

## Algorithm

Uses K-Merge sort (https://en.wikipedia.org/wiki/External_sorting#External_merge_sort).
