# microhttpd_project

# Build commands
gcc -I $HOME/include -L $HOME/lib main.c -l microhttpd -o main
export LD_LIBRARY_PATH=$HOME/lib

## Postgres installation
sudo apt-get install postgresql-all
sudo -u postgres createuser -s $USER
createdb mhd

