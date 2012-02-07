AUTOMAKE_OPTIONS= subdir-objects gnu
LDADD = $(DEPS_LIBS)
AM_CFLAGS = $(DEPS_CFLAGS)
ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}

bin_PROGRAMS=checkautoup
sbin_PROGRAMS=autoupdate
autoupdate_SOURCES=autoup/main.c
checkautoup_SOURCES=autoup/gui.c
