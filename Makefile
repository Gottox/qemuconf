VERSION = 0.0

all: qemuconf

qemuconf: qemuconf.c
	$(CC) $(LDFLAGS) $(CFLAGS) -DVERSION=\"$(VERSION)\" -o $@ $<

clean:
	rm qemuconf

.PHONY: clean
