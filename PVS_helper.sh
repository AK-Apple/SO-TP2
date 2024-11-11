#!/bin/bash

# Agregar encabezado a los archivos .c
read -p "Agregar encabezados? (y/n): " confirm2

if [[ $confirm2 =~ ^[Yy]$ ]]; then
    echo "Agregando encabezados a los archivos .c..."
    find . -name "*.c" | while read line; do 
        sed -i '1s/^\(.*\)$/\/\/ This is a personal academic project. Dear PVS-Studio, please check it.\n\1/' "$line"
        sed -i '2s/^\(.*\)$/\/\/ PVS-Studio Static Code Analyzer for C, C++ and C#: http:\/\/www.viva64.com\n\1/' "$line"
    done
    echo "Encabezados agregados."
else
    echo "Operación cancelada."
fi



# echo "Encabezados agregados previamente."

# Activar la licencia gratuita
echo "Activando la licencia gratuita de PVS-Studio..."
pvs-studio-analyzer credentials "PVS-Studio Free" "FREE-FREE-FREE-FREE"

# Realizar el análisis
echo "Iniciando el análisis con PVS-Studio..."
pvs-studio-analyzer trace -- make
pvs-studio-analyzer analyze

# Convertir el log a formato tasklist
echo "Generando el reporte..."
plog-converter -a '64:1,2,3;GA:1,2,3;OP:1,2,3' -t tasklist -o report.tasks PVS-Studio.log

#!/bin/bash

# Remover encabezados de los archivos .c
echo "Removiendo encabezados de los archivos .c..."

# Ask for confirmation
read -p "¿Desea remover los encabezados? (y/n): " confirm3

if [[ $confirm3 =~ ^[Yy]$ ]]; then
    find . -name "*.c" | while read -r line; do 
        # Use sed to remove the specific header lines
        sed -i '' -e '1{/^\/\/ This is a personal academic project. Dear PVS-Studio, please check it.$/d}' \
                   -e '2{/^\/\/ PVS-Studio Static Code Analyzer for C, C++ and C#: http:\/\/www.viva64.com$/d}' "$line"
        echo "Headers removed from: $line"
    done
    echo "Encabezados removidos."
else
    echo "Operación cancelada."
fi

echo "Análisis completo. El reporte se encuentra en 'report.tasks'."