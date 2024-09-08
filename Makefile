# hacktype - hackertyper.com for the terminal
# see LICENSE file for copyright and license details.

include config.mk

SRC = hacktype.c
OBJ = ${SRC:.c=.o}

all: hacktype

config.h:
	cp config.def.h config.h

hacktype.6: hacktype.scd
ifeq ($(shell command -v scdoc 2>/dev/null),)
	$(warning Missing dependency: scdoc. The man page will not be generated)
else
	scdoc <$< >$@
endif

.c.o:
	${CC} -c ${CFLAGS} $<

${OBJ}: config.h config.mk

hacktype: ${OBJ}
	${CC} -o $@ ${OBJ} ${LDFLAGS}

clean:
	rm -f hacktype ${OBJ}

install: all hacktype.6
	mkdir -p ${DESTDIR}${PREFIX}/bin
	cp -f hacktype ${DESTDIR}${PREFIX}/bin
	chmod 755 ${DESTDIR}${PREFIX}/bin/hacktype
	mkdir -p ${DESTDIR}${MANPREFIX}/man6
	[ ! -f hacktype.6 ] || cp -f hacktype.6 ${DESTDIR}${MANPREFIX}/man6/hacktype.6
	[ ! -f hacktype.6 ] || chmod 644 ${DESTDIR}${MANPREFIX}/man6/hacktype.6


uninstall:
	rm -f ${DESTDIR}${PREFIX}/bin/hacktype

.PHONY: all clean install uninstall
