CC = gcc
output = memtest
lib_objects = libmem.so
test_objects = test.o
objects = mem.o

$(output) : $(test_objects) $(lib_objects)
	$(CC)  test.c -lmem -L. -o $(output) -Wl,-rpath=${LD_LIBRARY_PATH}:.

$(lib_objects) : $(objects)
	$(CC) -shared -o $(lib_objects) $(objects)

$(objects) : mem.c
	$(CC) -c -fpic mem.c

clean :
	-rm -f $(output) *.o  *.a *.so core *~


