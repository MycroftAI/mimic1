#!/bin/sh
mkdir -p "m4" "config"
case `uname` in Darwin*) glibtoolize --copy ;;
  *) libtoolize --copy ;; esac
aclocal \
&& autoheader \
&& automake --add-missing \
&& autoconf
