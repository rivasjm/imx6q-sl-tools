
CC = arm-buildroot-linux-uclibcgnueabihf-gcc

all: gmutex first second

first: first.c
	$(CC) -o first first.c global_mutex.c -pthread -lrt

second: second.c
	$(CC) -o second second.c global_mutex.c -pthread -lrt

global_mutex.o: global_mutex.c global_mutex.h
	$(CC) -c global_mutex.c

gmutex: gmutex.o
	$(CC) -o gmutex gmutex.o -pthread -lrt

gmutex.o: gmutex.c
	$(CC) -c gmutex.c 

clean:
	@rm -f *.o gmutex first second test