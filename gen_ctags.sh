#! /bin/bash

find -name '*.c' -exec ctags {} +
find -name '*.cpp' -exec ctags {} +
find -name '*.h' -exec ctags {} +
