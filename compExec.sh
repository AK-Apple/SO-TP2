#!/bin/bash
NOMBRE=TP2

# abrir docker
docker run -d -v "$PWD":/root --security-opt seccomp:unconfined -ti --name $NOMBRE agodio/itba-so-multi-platform:3.0

# compilar
docker start $NOMBRE
docker exec -it $NOMBRE make clean -C/root/Toolchain
docker exec -it $NOMBRE make clean -C/root/
docker exec -it $NOMBRE make -C/root/Toolchain
docker exec -it $NOMBRE make -C/root/
docker stop $NOMBRE

# ejecutar:
./run.sh

# cerrar docker (cuando termine la ejecución):
docker rm $NOMBRE