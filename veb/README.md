## Description

Performs effective searching on sorted data using Van Emde Boas layout.

## Build

To build, you need SCons (http://www.scons.org/).
Run

`scons`

in the root of repository.

## Run

Input data format:

```
N
a b ... c
M
x y ... z
```

Second line has N numbers in increasing order -- input array.
Fourth line has M numbers in random order -- search queries.

## Experiments

On arrays of 100000 elements, speedup is around 34 %.

VEB running time is approx. 7809 s, where std::binary_search is 10541 s.

## Algorithm

Uses Van Emde Boas layout (https://en.wikipedia.org/wiki/Van_Emde_Boas_tree)
