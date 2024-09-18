#!/bin/bash
docker start TPEspecial
docker exec -it TPEspecial make clean -C/root/Toolchain
docker exec -it TPEspecial make clean -C/root/
docker exec -it TPEspecial make -C/root/Toolchain
docker exec -it TPEspecial make all -C/root/
# shellcheck disable=SC2046
docker exec -it TPEspecial chown -R $(id -u):$(id -g) /root/Image/
docker stop TPEspecial