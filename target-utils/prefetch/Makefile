# Example: make KERNEL=/root/dev/buildroot/sabrelite-rt/build/linux-custom CROSS=/root/dev/buildroot/sabrelite-rt/host/bin/arm-buildroot-linux-uclibcgnueabihf-

PWD := $(shell pwd)
obj-m += disable-l2-prefetch.o

all:
	make ARCH=arm CROSS_COMPILE=$(CROSS) -C $(KERNEL) SUBDIRS=$(PWD) modules
clean:
	make -C $(KERNEL) SUBDIRS=$(PWD) clean