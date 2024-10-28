# SO TP2 2024 2C
El objetivo de este TP es continuar el sistema operativo creado en arqui, agregando un scheduler de procesos, memoria dinamica, semaforos, pipes.

# Integrantes
- Luca Rossi 
- Alex Köhler
- Javier Liu

# Instrucciones de compilación y ejecución
Bajar el archivo de docker:
```sh
docker pull agodio/itba-so-multi-platform:3.0
```

Una vez que estés en el entorno de desarrollo, compila y ejecuta el programa utilizando el siguiente comando:
```sh
./compExec.sh
```

Otra alternativa es compilar y ejecutar por separado:
1. Entrar al docker:
```sh
docker run -v ${PWD}:/root --security-opt seccomp:unconfined -ti agodio/itba-so-multi-platform:3.0
```
2. Compilar:
```sh
cd root
make
```
3. Ejecutar FUERA DEL DOCKER con QEMU (en una terminal aparte o haciendo exit antes):
```sh
./run.sh
```
4. (Opcional) Agregar el flag -d para activar opciones de debugging con GDB:
```sh
./run.sh -d
```
# Comandos de la Shell
### help
Muestra la lista de comandos

### song
Pone música con beeps. Parámetros: song_id

###### Canciones: 
- song_id = 1: Eliminator
- song_id = 2: Iniciando SO
- song_id = 3: ¡Perdiste!

### Coming soon
