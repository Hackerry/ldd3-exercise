# Comment/uncomment the following line to disable/enable debugging
#DEBUG = y


# Add your debugging flag (or not) to CFLAGS
ifeq ($(DEBUG),y)
  DEBFLAGS = -O -g -DCOUNTER_DEBUG # "-O" is needed to expand inlines
else
  DEBFLAGS = -O2
endif

CFLAGS += $(DEBFLAGS)
CFLAGS += -I$(LDDINC)

ifneq ($(KERNELRELEASE),)
# call from kernel build system

counter-objs := main.o

obj-m	:= counter.o

else

# Old build examples
# KERNELDIR ?= /lib/modules/$(shell uname -r)/build
# PWD       := $(shell pwd)

# modules:
# 	$(MAKE) -C $(KERNELDIR) M=$(PWD) LDDINC=$(PWD)/../include modules

KDIR ?= /home/harrynjc/Desktop/ldd3_code/kernels/linux-6.14.4
PWD := $(shell pwd)

.PHONY: modules
modules:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

.PHONY: clean
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions

.PHONY: release
release: modules
	cp *.ko ../../initramfs/tmp/
	cp counter_load counter_unload ../../initramfs/tmp/
	cp test_*.sh ../../initramfs/tmp/

endif

depend .depend dep:
	$(CC) $(CFLAGS) -M *.c > .depend


ifeq (.depend,$(wildcard .depend))
include .depend
endif
