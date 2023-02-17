# This makefile creates a kernel file from three source files:
ifneq ($(wildcard /usr/bin/umps3),)
	UMPS3_DIR_PREFIX = /usr
	LIBDIR = $(UMPS3_DIR_PREFIX)/lib/x86_64-linux-gnu/umps3
else
	UMPS3_DIR_PREFIX = /usr/local
	LIBDIR = $(UMPS3_DIR_PREFIX)/lib/umps3
endif

INCDIR = $(UMPS3_DIR_PREFIX)/include/umps3/umps 
SUPDIR = $(UMPS3_DIR_PREFIX)/share/umps3

DEFS = pandos_const.h types.h ash.h pcb.h ns.h container_of.h hash.h hashtable_api.h hashtable.h list.h log2.h pandos_types.h $(INCDIR)/libumps.h Makefile #mettere tutti i file h  

OBJS = p1test.o ash.o pcb.o ns.o #tutti i file  .o che servono per compilare. 
CFLAGS = -ffreestanding -std=gnu99 -Wall -c -mips1 -mabi=32 -mfp32 \-mno-gpopt -G 0 -fno-pic -mno-abicalls 
#all'inizio di CFLAGS: $@ indica quello a sx dei due punti, 

LDAOUTFLAGS = -G 0 -nostdlib -T $(SUPDIR)/umpsaout.ldscript
LDCOREFLAGS = -G 0 -nostdlib -T $(SUPDIR)/umpscore.ldscript 

CC = mipsel-linux-gnu-gcc
LD = mipsel-linux-gnu-ld
AS = mipsel-linux-gnu-as -KPIC
EF = umps3-elf2umps
UDEV = umps3-mkdev

#main target
all: kernel.core.umps disk0.umps #

# use umps3-mkdev to create the disk0 device
disk0.umps:
	$(UDEV) -d disk0.umps
# create the kernel.core.umps kernel executable file
kernel.core.umps: kernel
	$(EF) -k kernel	

kernel: $(OBJS)
	$(LD) $(LDCOREFLAGS) $(LIBDIR)/crtso.o $(OBJS) \$(LIBDIR)/libumps.o -o kernel

%.o: %.c $(DEFS)
	$(CC) $(CFLAGS) $<
