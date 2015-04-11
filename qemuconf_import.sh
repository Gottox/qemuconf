#! /bin/sh
#
# mkconf.sh
# Copyright © 2015 tox <tox@rootkit>
#
# Distributed under terms of the MIT license.
#

mkconfig() {
	exe=$(readlink $1/exe)
	cwd=$(readlink $1/cwd)
	local lastopt=

	echo cwd "$cwd"
	printf "binary "
	tr "\0" "\n" < $1/cmdline | while read arg; do
		if [ -z "${arg%%-*}" ]; then
			[ "$lastopt" ] && echo
			printf "%s" "${arg#-}"
			lastopt=1
			continue;
		fi
		lastopt=

		if [ "${arg##*,*}" ]; then
			echo " ${arg}"
			continue
		fi
		echo :

		IFS=,
		for i in ${arg}; do
			printf "\t%s\n" "${i}";
		done
		unset IFS
	done
}

for proc in /proc/*/exe; do
	case $(readlink $proc) in
		*/qemu-*|*/qemu) : ;;
		*) continue;
	esac
	conf=$(mkconfig ${proc%/*})
	echo "----"
	echo "$conf"
	echo "----"
	read -p "Save as (enter to skip) " name
	[ "$name" ] && echo "$conf" > $name
done
