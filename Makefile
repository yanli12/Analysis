# Define (atleast) the following targets: all, clean
# all must build the executable file named mapreduce.
# clean must remove all object files and executable files created.
# Look at the introduction doc for information on how make a Makefile.
# This make file must build according to the structure described in the
# introduction doc, if it doesn't you will get a ZERO!

CFLAGS = -Wall -Werror
LDFLAGS = -lm
BINS = bin/mapreduce
DIR = bin/
DDIR = build/

all = BINS

mapreduce: main.o map_reduce.o
	@mkdir -p bin
	gcc $(CFLAGS) $(DDIR)main.o $(DDIR)map_reduce.o -o $(DIR)$@ $(LDFLAGS)

main.o: ./src/main.c
	@mkdir -p build
	gcc $(CFLAGS) -c $^ -o $(DDIR)$@

map_reduce.o: ./src/map_reduce.c
	@mkdir -p build
	gcc $(CFLAGS) -c $^ -o $(DDIR)$@
clean:
	rm -rf build bin