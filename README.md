QEMUCONF 1 qemuconf-0.2.1
=========================

NAME
----

qemuconf - simple qemu launcher with config file support

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

	binary /usr/bin/qemu-system-x86_64
	cwd    /var/qemu/generic
	smp    2
	m      1024
	enable-kvm
	drive:
		if      virtio
		file    hda.img
	net nic:
		vlan    0
		model   virtio
		macaddr 82:82:9D:AF:F0:1C
	net user:
		vlan    0
	vnc 127.0.0.1:1

This config file will execute the following command:

	/usr/bin/qemu-system-x86_64 -smp 2 -m 1024 -enable-kvm \
		-drive if=virtio,file=hda.img \
		-net nic,vlan=0,model=virtio,macaddr=82:82:9D:AF:F0:1C \
		-net user,vlan=0 -vnc 127.0.0.1:1

DIAGNOSTICS
-----------

To show the produced arguments list use the following:

	qemuconf -n config.conf
