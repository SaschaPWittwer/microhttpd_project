# microhttpd_project

# Build commands
gcc -I $HOME/include -L $HOME/lib main.c -l microhttpd -o main
export LD_LIBRARY_PATH=$HOME/lib

## Postgres installation
sudo apt-get install postgresql-all
sudo -u postgres createuser -s $USER
createdb mhd

## libgnunetpq installation
recompile gnunet
* ./configure --prefix=$HOME
add ´-I/usr/include/postgresql/´ to build command
add the following includes to your C File: 
* ´#include <libpq-fe.h>´
* ´#include <gnunet/platform.h>´
* ´#include <gnunet/gnunet_pq_lib.h>´


