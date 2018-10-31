PWD=$(shell pwd)

obj-m := test_20161015.o

all:
	make ARCH=arm CROSS_COMPILE=/home/fujii/gcc-linaro-5.3.1-2016.05-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf- -C /home/fujii/FPGA-SoC-Linux/scripts/linux-4.4.7-armv7-fpga M=$(PWD) modules
clean:
	rm -f test_20161015.o

