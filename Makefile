VERSION = 0.0
BINARY = qemu-system-x86_64

all: qemuconf

qemuconf: qemuconf.c
	$(CC) $(LDFLAGS) $(CFLAGS) -DVERSION=\"$(VERSION)\" -DBINARY=\"$(BINARY)\" -o $@ $<

clean:
	rm qemuconf

.PHONY: clean
