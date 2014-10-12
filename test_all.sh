#!/bin/bash
file=4294967296

head -c $file < /dev/urandom > input_bin

for b in 268435456 536870912 1073741824 ;
do
    for d in 4 8 20 ;
    do
        echo "$b $d" ;
        time ./ext_sort input_bin output_bin -b "$b" -d "$d" ;
    done ;
done
