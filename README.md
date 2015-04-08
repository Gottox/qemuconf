QEMUCONF 1 qemuconf-0.1
=======================

NAME
----

qemuconf - a simple config parser for qemu

SYNOPSIS
--------

**qemuconf** [**-q** `exec`] *config-file* [-- qemu-options ...]

**qemuconf** [**-v**]

DESCRIPTION
-----------

**qemuconf** executes qemu with arguments compiled from a config file.

OPTIONS
-------

**-q** `exec`
Sets the the default binary. This option may be overwritten by the **binary**
option in the config file.

**-n**
instead of executing the argument list will be printed to stdout.

**-v**
prints version and exits.

EXAMPLES
--------

	cwd    /var/qemu/void
	binary /usr/bin/qemu-system-x86_64
	smp    8
	m      1024
	enable-kvm
	cdrom ../void/void-live-x86_64-latest.iso
	drive:
		if      virtio
		file    /dev/vg0/vm1
	net: nic
		vlan    0
		model   virtio
		macaddr 82:82:9D:AF:F0:1C
	net: user
		vlan    0
	vnc 127.0.0.1:1

This config file will execute the following command:

	/usr/bin/qemu-system-x86_64 -smp 8 -m 1024 \
		-enable-kvm -cdrom ../void/void-live-x86_64-latest.iso \
		-drive if=virtio,file=/dev/vg0/vm1 \
		-net nic,vlan=0,model=virtio,macaddr=82:82:9D:AF:F0:1C \
		-net user,vlan=0 -vnc 127.0.0.1:1

DIAGNOSTICS
-----------

To show the produced arguments list use the following:

	qemuconf -n config.conf
