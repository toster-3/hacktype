# hacktype version
VERSION = 1

# paths
PREFIX = /usr/local
MANPREFIX = ${PREFIX}/share/man

# libs
LIBS = -lc -lcurses

# flags
CPPFLAGS = -DVERSION=\"${VERSION}\" -D_XOPEN_SOURCE=600
#CFLAGS += -std=c99 -pedantic -Wall -Os ${CPPFLAGS}
CFLAGS += -g -std=c99 -pedantic -Wall ${CPPFLAGS}
LDFLAGS += ${LIBS}

# compiler
CC ?= cc
