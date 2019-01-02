#!/bin/bash
gcc -g -I$HOME/include -I/usr/include/postgresql/ -I./ -L$HOME/lib -lmicrohttpd -lz -lgnunetpq -lpq -ljansson -O0 basicauthentication.c dbutil.c -o basicauthentication
export LD_LIBRARY_PATH=$HOME/lib
./basicauthentication
