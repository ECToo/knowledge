#!/bin/bash
aclocal
libtoolize
autoconf
automake --add-missing --gnu
./configure
