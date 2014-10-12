#!/bin/sh

head -c $1 < /dev/urandom > input_bin
time ./ext_sort input_bin output_bin -b $2 -d $3
./bin2text output_bin output_text
./bin2text input_bin input_txt
cat input_txt | sort -n > expected_output
diff output_text expected_output
