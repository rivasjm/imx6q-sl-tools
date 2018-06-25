
CC = arm-buildroot-linux-uclibcgnueabihf-gcc

test: global_mutex.o test.o
	$(CC) -o test test.o global_mutex.o -pthread -lrt

global_mutex.o: global_mutex.h global_mutex.c
	$(CC) -c global_mutex.c

test.o: test.c
	$(CC) -c test.c

clean:
	@rm -f *.o test