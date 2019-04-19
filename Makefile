export
CCPREFIX=arm-linux-gnueabihf-
sysroot := "/home/sinamalek/linux"        
obj-m += proj3_pt1.o

all:
	make ARCH=arm CROSS_COMPILE=${CCPREFIX} -C $(sysroot) M=$(PWD) modules

clean:
	make -C $(sysroot) M=$(PWD) clean
