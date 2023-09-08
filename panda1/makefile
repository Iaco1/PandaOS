# Directory prefix for UMPS3
ifneq ($(wildcard /usr/bin/umps3),)
UMPS3_DIR_PREFIX = /usr
LIBDIR = $(UMPS3_DIR_PREFIX)/lib/x86_64-linux-gnu/umps3
else
UMPS3_DIR_PREFIX = /usr/local
LIBDIR = $(UMPS3_DIR_PREFIX)/lib/umps3
endif

# Directories and files
INCDIR = $(UMPS3_DIR_PREFIX)/include/umps3/umps
SUPDIR = $(UMPS3_DIR_PREFIX)/share/umps3
DEFS = $(INCDIR)/libumps.h Makefile
OBJS = ash.o crtso.o exceptions.o hash.o interrupts.o libumps.o main.o ns.o pcb.o p2test.04.o

# Compilation flags
CFLAGS = -ffreestanding -ansi -Wall -c -mips1 -mabi=32 -mfp32 -mno-gpopt -G 0 -fno-pic -mno-abicalls

# Linking flags
LDAOUTFLAGS = -G 0 -nostdlib -T $(SUPDIR)/umpsaout.ldscript

# Compiler and linker
CC = mipsel-linux-gnu-gcc
LD = mipsel-linux-gnu-ld
AS = mipsel-linux-gnu-as -KPIC
EF = umps3-elf2umps
UDEV = umps3-mkdev

# Main target
all: p2test.umps

# Compilation rules
%.o: %.c $(DEFS)
	$(CC) $(CFLAGS) $<

# Linking rule
%.t: %.o $(OBJS)
	$(LD) $(LDAOUTFLAGS) $(LIBDIR)/crti.o $< $(LIBDIR)/libumps.o -o $@

# ELF to UMPS3 binary conversion rule
%.t.aout.umps: %.t
	$(EF) -a $<

# Final target rule
%.umps: %.t.aout.umps
	$(UDEV) -f $@ $<

CFLAGS += -w

clean:
	rm -f ash.c crtso.o exceptions.h hashtable.h libumps.o main.c ns.h output pandos_types.h types.h ash.h hash.h interrupts.c list.h p2test.04.c pcb.c container_of.h exceptions.c hashtable_api.h interrupts.h log2.h ns.c pandos_const.h pcb.h

