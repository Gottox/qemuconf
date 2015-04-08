BINARY ?= qemu-system-x86_64
PREFIX ?= /usr/local
ETCDIR ?= /etc
MANPREFIX ?= $(PREFIX)/share/man
VERSION = $(shell head -n 1 README.md | cut -d- -f 2)

all: qemuconf

qemuconf: qemuconf.c
	$(CC) $(LDFLAGS) $(CFLAGS) -DVERSION=\"$(VERSION)\" -DBINARY=\"$(BINARY)\" -o $@ $<

clean:
	rm -f qemuconf qemu@.service qemu.runit

qemuconf.1: README.md
	pandoc -f markdown_github -t man $< | \
		sed "1s/\.SH/.TH/" > $@

qemu@.service: qemu@.service.in
	sed "s#PREFIX#$(PREFIX)#g;s#ETCDIR#$(ETCDIR)#" $< > $@

qemu.runit: qemu.runit.in
	sed "s#PREFIX#$(PREFIX)#g;s#ETCDIR#$(ETCDIR)#" $< > $@

install:
	mkdir -p $(DESTDIR)$(PREFIX)/bin $(DESTDIR)$(MANPREFIX)/man1 \
		$(ETCDIR)/qemu.d
	cp -f qemuconf $(DESTDIR)$(PREFIX)/bin
	chmod 755 $(DESTDIR)$(PREFIX)/bin/qemuconf
	cp -f qemuconf.1 $(DESTDIR)$(MANPREFIX)/man1/
	chmod 644 $(DESTDIR)$(MANPREFIX)/man1/qemuconf.1

install-systemd: qemu@.service install
	mkdir -p $(DESTDIR)$(PREFIX)/lib/systemd/system
	cp -f $< $(DESTDIR)$(PREFIX)/lib/systemd/system
	chmod 644 $(DESTDIR)$(PREFIX)/lib/systemd/system/$<

install-runit: qemu.runit install
	mkdir -p $(DESTDIR)$(PREFIX)/sv/qemu-generic
	cp -f $< $(DESTDIR)$(PREFIX)/sv/qemu-generic/run
	chmod 644 $(DESTDIR)$(PREFIX)/sv/qemu-generic/run

uninstall:
	rm -rf $(DESTDIR)$(MANPREFIX)/man1/qemuconf.1 $(DESTDIR)$(PREFIX)/sv/qemu-generic \
		$(DESTDIR)$(PREFIX)/lib/systemd/system/qemu@.service $(DESTDIR)$(PREFIX)/bin/qemuconf

test: qemuconf
	@test/test.sh

.PHONY: clean install install-systemd install-runit uninstall test
