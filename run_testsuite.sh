#!/bin/sh
./autogen.sh
./configure  CFLAGS="--coverage --no-inline" LDFLAGS="--coverage" || exit 1
make || exit 1
make check || exit 1
./do_gcov.sh
