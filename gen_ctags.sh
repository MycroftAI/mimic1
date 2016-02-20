#! /bin/bash

#clear the file
> tags

#append to tags file
find -name '*.c' -exec ctags --append {} +
find -name '*.cpp' -exec ctags --append {} +
find -name '*.h' -exec ctags --append {} +
