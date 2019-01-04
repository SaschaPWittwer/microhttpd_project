# collects all source files in the src directory in the src variable
src = $(wildcard src/*.c)
# transforms the contents of the src variable, changing all file suffixes from .c to .o
obj = $(src:.c=.o)
CC = gcc
BIN = microserver

# variable called LDFLAGS for the list of libraries required during linking
# LDFLAGS is conventionally used for this usage.
LDFLAGS = -lmicrohttpd -lz -lgnunetpq -lpq -ljansson -L$(HOME)/lib

# while similarly CFLAGS and can be used to pass flags to the C compiler
CFLAGS = -Wall -g -O0 -I$(HOME)/include -I/usr/include/postgresql -I./src

$(BIN): $(obj)
	$(CC) -o $@ $^ $(LDFLAGS)

# The clean rule is marked as phony, because its target is not an actual file that will be generated,
# but just an arbitrary name that we wish to use for executing this rule.
.PHONY: clean
clean:
	rm -f $(obj) $(BIN)
