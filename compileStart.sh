#!/bin/bash
gcc -g -I$HOME/include -I/usr/include/postgresql/ -L$HOME/lib main.c -lmicrohttpd -lz -lgnunetpq -lpq -o main
export LD_LIBRARY_PATH=$HOME/lib
./main
