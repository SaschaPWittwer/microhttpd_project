#!/bin/bash
gcc -g -I$HOME/include -I/usr/include/postgresql -I./include -L$HOME/lib -lmicrohttpd -lz -lgnunetpq -lpq -ljansson -O0 src/basicauthentication.c src/dbutil.c -o basicauthentication
export LD_LIBRARY_PATH=$HOME/lib
./basicauthentication
