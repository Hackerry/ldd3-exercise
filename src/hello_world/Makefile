# file Makefile
# KDIR ?= /lib/modules/$(shell uname -r)/build
KDIR ?= /home/harrynjc/Desktop/ldd3_code/kernels/linux-6.14.4
PWD := $(shell pwd)

.PHONY: modules
modules:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

.PHONY: clean
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
