#!/bin/bash

read -p "¿Seguro que desea remover los encabezados? (y/n): " confirm3

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
