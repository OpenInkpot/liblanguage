lib_LTLIBRARIES = liblanguage.la
liblanguage_la_SOURCES = liblanguage.c
include_HEADERS = liblanguage.h

AM_CFLAGS = -std=gnu99 -Wall -DDATADIR='"$(datadir)"'

INCLUDES = @LIBLOPS_CFLAGS@
liblanguage_la_LDFLAGS = @LIBLOPS_LIBS@ -no-undefined -version-info 1:0:0
