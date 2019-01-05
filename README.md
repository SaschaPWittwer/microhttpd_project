# microserver
The microserver aims to provide a portable REST-API implemented on top of [GNU Libmicrohttpd](https://www.gnu.org/software/libmicrohttpd/).\
Full [documentation](https://www.gnu.org/software/libmicrohttpd/manual/libmicrohttpd.html#microhttpd_002dpost) of MHD is available the official GNU site. A [tutorial](https://www.gnu.org/software/libmicrohttpd/tutorial.html) is also available.

# Build
To build the project just run ```make``` in the terminal. To cleanup any generated build artefacts run ```make clean```.

export LD_LIBRARY_PATH=$HOME/lib

## Postgres installation
sudo apt-get install postgresql-all\
sudo -u postgres createuser -s $USER\
createdb mhd\

## libgnunetpq installation
recompile gnunet
* ./configure --prefix=$HOME
add ´-I/usr/include/postgresql/´ to build command
add the following includes to your C File: 
* ´#include <libpq-fe.h>´
* ´#include <gnunet/platform.h>´
* ´#include <gnunet/gnunet_pq_lib.h>´


