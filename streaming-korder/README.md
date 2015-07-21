## Description

Computes k-order statistics in streaming data.
Stream should be repeated twice (see exampl).

## Build

To build, you need SCons (http://www.scons.org/).
Run

`scons`

in the root of repository.


## Run

Single argument is `k` -- statistics to be computed.
`k` should be in range `[1,N]`, where `N` is size of the stream.
Program reads text data from input.
Reads integers splitted by spaces. Stream ends with integer 0.
After that, stream should be repeated once modre (again ending with integer 0).

## Example

`./sko 5`
Input:
`> 1 6 2 6 4 5 2 6 3 0 1 6 2 6 4 5 2 6 3 0`
Output:
`> 4`

## Algorithm

Uses Munro-Patterson algorithm to compute k-order statistics
(https://polylogblog.files.wordpress.com/2009/08/80munro-median.pdf).
