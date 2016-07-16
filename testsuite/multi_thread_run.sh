#!/bin/sh

export OMP_NUM_THREADS=100 && $(top_builddir)/testsuite/multi-thread

