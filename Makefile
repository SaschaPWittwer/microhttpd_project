# collects all source files in the src directory in the src variable
src = $(wildcard src/*.c)
# transforms the contents of the src variable, changing all file suffixes from .c to .o
obj = $(src:.c=.o)
CC = gcc
BIN = microserver

# List of all pre-defined Variables
# https://www.gnu.org/software/make/manual/html_node/Implicit-Variables.html

# Library flags or names given to compilers when they are supposed to invoke the linker
LDLIBS = -L$(HOME)/lib

# variable called LDFLAGS for the list of libraries required during linking
LDFLAGS = -lmicrohttpd -lz -lgnunetpq -lpq -ljansson -ljwt

# Extra flags to give to the C preprocessor and programs that use it
CPPFLAGS = -I$(HOME)/include -I/usr/include/postgresql -I./src

# Extra flags to give to the C compiler
CFLAGS = -std=c99 -Wall -g -O0

$(BIN): $(obj)
	$(CC) -o $@ $^ $(LDLIBS) $(LDFLAGS)

# The clean rule is marked as phony, because its target is not an actual file that will be generated,
# but just an arbitrary name that we wish to use for executing this rule.
.PHONY: clean
clean:
	rm -f $(obj) $(BIN)
