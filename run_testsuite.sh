#!/bin/sh

./configure --enable-gcov || exit 1
make || exit 1
make test || exit 1

