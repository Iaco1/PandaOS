XT_PRG_PREFIX = mipsel-linux-gnu-
CC = $(XT_PRG_PREFIX)gcc
LD = $(XT_PRG_PREFIX)ld

ifneq ($(wildcard /usr/bin/umps3),)
	UMPS3_DIR_PREFIX = /usr
else
	UMPS3_DIR_PREFIX = /usr/local
endif

UMPS3_DATA_DIR = $(UMPS3_DIR_PREFIX)/share/umps3
UMPS3_INCLUDE_DIR = $(UMPS3_DIR_PREFIX)/include/umps3
UMPS3_INCLUDE_DIR2 = $(UMPS3_DIR_PREFIX)/include/umps3/umps
UMPS3_INCLUDE_DIR3 = /usr/include

CFLAGS_LANG = -ffreestanding
CFLAGS_MIPS = -mips1 -mabi=32 -mno-gpopt -G 0 -mno-abicalls -fno-pic -mfp32
CFLAGS = $(CFLAGS_LANG) $(CFLAGS_MIPS) -I$(UMPS3_INCLUDE_DIR) -I$(UMPS3_INCLUDE_DIR3) -I$(UMPS3_INCLUDE_DIR2) -Wall -O0

LDFLAGS = -G 0 -nostdlib -T $(UMPS3_DATA_DIR)/umpscore.ldscript

SRC = $(wildcard *.c)
OBJS = $(SRC:.c=.o)	# Auto-updated obj listing

VPATH = $(UMPS3_DATA_DIR)

.PHONY : all clean

all : kernel.core.umps clean
	@echo "** All successful! Good Luck!! :) [remember to set the TLB Floor Address to 0x8000.000 in UMPS3!!]"

kernel.core.umps : kernel
	umps3-elf2umps -k $<

kernel : $(OBJS) crtso.o libumps.o
	$(LD) -o $@ $^ $(LDFLAGS)
	@echo "** Compilation successful!"

clean :
	@echo "** Cleaning object files..."
	-rm -f *.o

clean_all :
	@echo "** Cleaning all generated umps files..."
	-rm -f *.o kernel *.umps

%.o : %.S
	$(CC) $(CFLAGS) -c -o $@ $<

