
# Author    KMS - Martin Dubois, P. Eng.
# Copyright (C) 2025 KMS
# License   http://www.apache.org/licenses/LICENSE-2.0
# Product   DrvDMA-Samples
# File      D_Ethernet/Makefile

EXTRA_CFLAGS := 

obj-m += D_Ethernet.o

D_Ethernet-objs := \
    Adapter_L.o    \
	Driver_L.o     \
	DrvDMA_Glue.o

ccflags-y := -D_KMS_LINUX_ -I /usr/local/DrvDMA-3.0/inc

ldflags-y := --whole-archive /usr/src/DrvDMA-3.0/DrvDMA_K.a

# ===== Rules ===============================================================

# .cpp.o:
#	g++ -c $(CFLAGS) -o $@ $<

# ===== Targets =============================================================

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
