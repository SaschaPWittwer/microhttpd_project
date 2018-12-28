#!/bin/bash

gcc -g -I$HOME/include -I/user/include/postgresql -L$HOME/lib main.c -l microhttpd -o main
export LD_LIBRARY_PATH=$HOME/lib
./main
