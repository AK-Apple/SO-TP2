#!/bin/bash
NOMBRE=TP2

MAKE_CMD="make"
if [[ "$1" == "buddy" ]]; then
    MAKE_CMD="make buddy"
fi

# abrir docker
docker run -d -v "$PWD":/root --security-opt seccomp:unconfined -ti --name $NOMBRE agodio/itba-so-multi-platform:3.0

# compilar
docker start $NOMBRE
docker exec -it $NOMBRE make clean -C/root/
docker exec -it $NOMBRE $MAKE_CMD -C/root/
docker stop $NOMBRE

# ejecutar:
./run.sh

# cerrar docker (cuando termine la ejecución):
docker rm $NOMBRE