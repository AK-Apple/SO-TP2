# SO TP2 2024 2C

El objetivo de este TP es continuar el sistema operativo creado en arqui, agregando un scheduler de procesos, memoria dinamica, semaforos, pipes.

## Integrantes

- Luca Rossi
- Alex Köhler
- Javier Liu

## Instrucciones de compilación y ejecución

### Opción 1: con compExec

1. Bajar el archivo de docker:

```sh
docker pull agodio/itba-so-multi-platform:3.0
```

2. Una vez que estés en el entorno de desarrollo, compila y ejecuta el programa utilizando el siguiente comando:

```sh
./compExec.sh
```

### Otra alternativa es compilar y ejecutar por separado:

1. Bajar el archivo de docker:

```sh
docker pull agodio/itba-so-multi-platform:3.0
```

2. Entrar al docker:

```sh
docker run -v ${PWD}:/root --security-opt seccomp:unconfined -ti agodio/itba-so-multi-platform:3.0
```

3. Compilar:

```sh
cd root
make
```

4. Ejecutar FUERA DEL DOCKER con QEMU (en una terminal aparte o haciendo exit antes):

```sh
./run.sh
```

5. (Opcional) Agregar el flag -d para activar opciones de debugging con GDB:

```sh
./run.sh -d
```

## Comandos Built-in de la Shell (no crean procesos)

### help

Muestra la lista de comandos

### song \<song_id>

Pone música con beeps según \<song_id>.

#### Canciones

- song_id = 1: Eliminator
- song_id = 2: Iniciando SO
- song_id = 3: ¡Perdiste!

### time

Muestra la hora.

### eliminator

Ejecuta el juego eliminator.

### size \<font_size>

Cambia el tamaño de letra a \<font_size>, el cual es un número del 1 al 5.

### div \<num> \<den>

Devuelve \<num> / \<den>.

### invalidopcode

Ejecuta código invalido (y muestra excepción).

### inforeg

Muestra los registros guardados. (Presiona 'Left Alt' para guardar registros.)

### clear

Limpia toda la pantalla.

### ps

Lista la información de los procesos.

### kill \<pid>

Mata el proceso de pid = \<pid>.

### mem

Imprime el estado de la memoria. Utiliza el siguiente formato:

\<memoria ocupada>:bytes | \<memoria libre>:free | \<memoria ocupada>:bytes ...

Luego imprime:

- Total memory (memoria total)
- Used memory (memoria usada)
- Free memory (memoria libre)
- Internal fragmentation (fragmentación interna)
- Largest free block (bloque libre más largo)

### fg \<pid>

Manda el proceso con pid = \<pid> a foreground.

### block \<pid>

Cambia el estado del proceso de pid = \<pid>:

- Si estaba BLOCKED (bloqueado), pasa a READY (listo)
- Si estaba READY (listo), pasa a BLOCKED (bloqueado)

### nice \<pid> \<prio>

Cambia la prioridad del proces de pid = \<pid>. \<prio> indica el nivel de prioridad (de 1 a 5).

## Comandos de la Shell que crean procesos

### \<comando> &

Escribir '&' al final del comando para ejecutaro en background.

### testproc \<max_proc>

Ejecuta test de proceso.

### testprio

Ejecuta test de prioridades.

### testsync \<count> \<sem>

Ejecuta test de sincronización.

### testmman \<max> \<smart>

Ejecuta test de memoria.

### loop \<secs_wait> \<msg>

Imprime su ID con un saludo cada determinado tiempo.

## TODO

Sacar eliminator de Built-in.

Pasar todos los textos en inglés a español.

Explicar parámetros en los testeos.
