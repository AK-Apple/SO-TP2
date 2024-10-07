#!/bin/bash
if [ "$1" = "gdb" ]; then
 qemu-system-x86_64 -s -S -hda Image/x64BareBonesImage.qcow2 -m 512 -d int
else
 qemu-system-x86_64 -hda Image/x64BareBonesImage.qcow2 -m 512
fi

# comando para correr el docker:
# docker run -v ${PWD}:/root --security-opt seccomp:unconfined -ti agodio/itba-so-multi-platform:3.0