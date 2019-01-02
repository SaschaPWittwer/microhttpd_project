#!/bin/bash
gcc -g -I$HOME/include -I/usr/include/postgresql/ -L$HOME/lib basicauthentication.c -lmicrohttpd -lz -lgnunetpq -lpq -ljansson -O0 -o basicauthentication
export LD_LIBRARY_PATH=$HOME/lib
./basicauthentication
