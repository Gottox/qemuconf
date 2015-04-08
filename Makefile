BINARY ?= qemu-system-x86_64
PREFIX ?= /usr/local
MANPREFIX ?= $(PREFIX)/share/man
VERSION = $(shell head -n 1 README.md | cut -d- -f 2)

all: qemuconf

qemuconf: qemuconf.c
	$(CC) $(LDFLAGS) $(CFLAGS) -DVERSION=\"$(VERSION)\" -DBINARY=\"$(BINARY)\" -o $@ $<

clean:
	rm -f qemuconf

qemuconf.1: README.md
	pandoc -f markdown_github -t man $< | \
		sed "1s/\.SH/.TH/" > $@

install:
	mkdir -p ${DESTDIR}${PREFIX}/bin ${DESTDIR}${MANPREFIX}/man1
	cp -f qemuconf ${DESTDIR}${PREFIX}/bin
	chmod 755 ${DESTDIR}${PREFIX}/bin/qemuconf
	cp -f qemuconf.1 ${DESTDIR}${MANPREFIX}/man1/
	chmod 644 ${DESTDIR}${MANPREFIX}/man1/qemuconf.1

uninstall:
	rm -f ${DESTDIR}${MANPREFIX}/man1/qemuconf.1 \
		${DESTDIR}${PREFIX}/bin/qemuconf

.PHONY: clean install uninstall
