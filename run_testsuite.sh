#!/bin/sh

./configure --enable-gcov || exit 1
make || exit 1
(cd unittests && make && ./run_all_tests)


