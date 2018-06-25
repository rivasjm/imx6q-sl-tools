test: test.o test_man.o
	arm-buildroot-linux-uclibcgnueabihf-gcc -o test test.o
	arm-buildroot-linux-uclibcgnueabihf-gcc -o test_man test_man.o

test.o: test.c test_man.c
	arm-buildroot-linux-uclibcgnueabihf-gcc -c test.c
	arm-buildroot-linux-uclibcgnueabihf-gcc -c test_man.c