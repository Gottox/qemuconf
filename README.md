# Qemu Wrapper

very simple configuration wrapper for qemu to be used with systemd and runit

virsh is too XML-ly, lxc does not give you the kernel space and plain qemu
isn't integrated into the system at all? - Try qemuconf, a thin layer around
qemu which generates qemu argument lists from simple config files.

It supports all config options of qemu plus the following:

* `cwd` change to working directory before calling qemu
* `binary /path/to/qemu` the path to the qemu binary.
* `. /path/` includes a file.

## example config

```
binary /usr/bin/qemu-system-x86_64
m 512
enable-kvm
no-shutdown
hda /path/to/image
net nic
    vlan 0
    macaddr A7:6C:8F:40:52:6A
net user
    vlan 0

cdrom /path/to/iso
```
calls ```-enable-kvm -no-shutdown -hda /path/to/image -net nic,vlan=0,macaddr=A7:6C:8F:40:52:6A -net user,vlan=0 -cdrom /path/to/iso```
