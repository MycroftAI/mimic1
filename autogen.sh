#!/bin/sh
mkdir -p "m4" "config"
libtoolize --copy \
&& aclocal \
&& autoheader \
&& automake --add-missing \
&& autoconf
