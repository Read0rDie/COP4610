output = memtest
lib_objects = libmem.so
objects = mem.o

$(output) : $(lib_objects)
	gcc main.c -lmem -L -o $(output)

$(lib_objects) : $(objects) 
	gcc -shared -o $(output) $(objects)
	
mem.o : mem.c
	gcc -c -fpic mem.c

clean :
	-rm -f $(output) *.o  core *~ 
