
ifeq ($(CROSS_COMPILE),)
CROSS_COMPILE=arm-none-linux-gnueabi-
endif

# Base address to load the program
RAM_BASE=0x00100010

AS = $(CROSS_COMPILE)as
CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)ld
AR = $(CROSS_COMPILE)ar
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdump 

.PHONY: clean all dump

gccincdir := $(shell $(CC) -print-file-name=include)

# -O0          : Disable optimizations
# -g           : Generate debug info
# -ffixed-r8   : Don't touch register r8
CFLAGS = -nostdinc \
		-fno-common \
		-fno-builtin \
		-ffixed-r8 \
		-msoft-float \
		-ffreestanding \
		-I./ \
		-DTEXT_BASE=$(RAM_BASE) \
		-D__KERNEL__ \
		-O0 \
		-g -Wall \
		-marm -mcpu=arm1176jzf-s \
		-isystem $(gccincdir)

# Name of target program
PROG=pure-usb

# List of all *c sources
CSRC=main.c ns16550.c vsprintf.c console.c string.c ctype.c asm/div0.c asm/hang.c asm/div64.c

#List of all *S (asm) sources
ASRC=start.S asm/_ashldi3.o asm/_ashrdi3.o asm/_divsi3.o asm/_lshrdi3.o asm/_modsi3.o asm/_udivsi3.o asm/_umodsi3.o

COBJ = $(subst .c,.o,$(CSRC))
AOBJ = $(subst .S,.o,$(ASRC))

all: $(PROG).bin
	
$(AOBJ): %.o : %.S
	$(CC) $(CFLAGS) -c -o $@ $<
	
$(COBJ): %.o : %.c
	$(CC) $(CFLAGS) -c -o $@ $<
	
$(PROG).elf: $(COBJ) $(AOBJ)
	$(LD) -T boot.lds -Ttext=$(RAM_BASE) $^ -o $@

$(PROG).bin: $(PROG).elf
	$(OBJCOPY) -v -O binary $^ $@

clean:
	rm -f *.a
	rm -f *.o  asm/*.o
	rm -f *.elf
	rm -f *.v
	rm -f *.bin
	rm -f *.md5
	rm -f *.hex
	rm -f *.dmp

# Show the disassembly
disassemble: $(PROG).elf
	$(OBJDUMP) -d $(PROG).elf


