
obj-m	:= simproc.o

#KERNELDIR ?= ~/armsystem/kernel/linux-4.9.30/
KERNELDIR ?= /lib/modules/$(shell uname -r)/build
PWD       := $(shell pwd)

all: default

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules


clean:
	rm -rf *.o .depend .*.o .*.cmd *.ko *.mod.c .tmp_versions modules.order Module.symvers *.mod

