# Example: make KERNEL=/root/dev/buildroot/sabrelite-rt/build/linux-custom CROSS=/root/dev/buildroot/sabrelite-rt/host/bin/arm-buildroot-linux-uclibcgnueabihf-

PWD := $(shell pwd)
obj-m += disable_l1_coherency.o

all: test
	make ARCH=arm CROSS_COMPILE=$(CROSS) -C $(KERNEL) SUBDIRS=$(PWD) modules

test: test.o
	arm-buildroot-linux-uclibcgnueabihf-gcc -o test test.o

test.o:
	arm-buildroot-linux-uclibcgnueabihf-gcc -c test.c

clean:
	make -C $(KERNEL) SUBDIRS=$(PWD) clean
	rm test.o