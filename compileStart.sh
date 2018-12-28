#!/bin/bash

gcc -g -I$HOME/include -L$HOME/lib main.c -l microhttpd -o main
export LD_LIBRARY_PATH=$HOME/lib
./main
