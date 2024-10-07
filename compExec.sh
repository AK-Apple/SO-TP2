#!/bin/bash
NOMBRE=TP2

sudo docker run -d -v "$PWD":/root --security-opt seccomp:unconfined -ti --name $NOMBRE agodio/itba-so-multi-platform:3.0
# sudo docker run -d -v "$PWD":/root --security-opt seccomp:unconfined -ti --name $NOMBRE agodio/itba-so:1.0

# permisos que no entiendo para qué sirven
# groupadd -g 1000 -o alex
# useradd -m -u 1000 -g 1000 -o -s /bin/bash alex

docker start $NOMBRE
docker exec -it $NOMBRE make clean -C/root/Toolchain
docker exec -it $NOMBRE make clean -C/root/
docker exec -it $NOMBRE make -C/root/Toolchain
docker exec -it $NOMBRE make -C/root/
docker stop $NOMBRE

# Set FLAG based on the input argument
FLAG=""
[[ "$1" = "-d" ]] && FLAG="-s -S -d int"

# Set common qemu options
QEMU_CMD="qemu-system-x86_64 $FLAG -hda Image/x64BareBonesImage.qcow2 -m 512"

if [[ "$(uname)" == "Linux" ]]; then
    # Linux specific execution
    sudo $QEMU_CMD
else
    # macOS specific execution
    export AUDIO_DRIVER="coreaudio"
    $QEMU_CMD -audiodev $AUDIO_DRIVER,id=audio0 -machine pcspk-audiodev=audio0
fi

# para cerrar:
docker rm $NOMBRE

# ni idea qué es esto:
# export GCC_COLORS="error=01;31:warning=01;35:note=01;36:caret=01;32:locus=01:quote=01"
# export LS_COLORS="rs=0:di=01;34:ln=01;36:mh=00:pi=40;33:so=01;35:do=01;35:bd=40;33;01:cd=40;33;01:or=40;31;01:m"
# alias ls='ls --color=auto'
# cd /root
