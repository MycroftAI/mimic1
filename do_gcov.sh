#!/bin/sh

for direct in $(find . -type d); do
   (cd "$direct"; 
    for file in $(ls | grep "\\.gcno$"); do
        echo "$PWD";
        gcov -b "$file";
    done)
done
