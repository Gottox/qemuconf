VERSION = 0.0
BINARY = qemu-system-x86_64

all: qemuconf

qemuconf: qemuconf.c
	$(CC) $(LDFLAGS) $(CFLAGS) -DVERSION=\"$(VERSION)\" -DBINARY=\"$(BINARY)\" -o $@ $<

clean:
	rm -f qemuconf

qemuconf.1: README.md
	pandoc -f markdown_github -t man $< | \
		sed "1s/\.SH/.TH/" > $@

.PHONY: clean
