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

CPPFLAGS = -DTEXT_BASE=$(RAM_BASE) \
		-D__KERNEL__

CFLAGS = -nostdinc \
		-fno-common \
		-fno-builtin \
		-ffixed-r8 \
		-msoft-float \
		-ffreestanding \
		-mabi=aapcs-linux \
		-mno-thumb-interwork \
		-Wstrict-prototypes \
		-fno-stack-protector \
		-I./ \
		-g -Wall \
		-marm \
		-mtune=arm1176jzf-s \
		-isystem $(gccincdir) \
		-Os

ASFLAGS = $(CFLAGS)

AOBJ=start.o asm/_ashldi3.o asm/_ashrdi3.o asm/_divsi3.o asm/_lshrdi3.o asm/_modsi3.o asm/_udivsi3.o asm/_umodsi3.o

COBJ_COMMON=main.o vsprintf.o console.o string.o ctype.o asm/div0.o asm/div64.o ehci-hcd.o ehci-fusb20.o usb.o ohci-test.o time.o

COBJ_BOARD=$(COBJ_COMMON) ns16550.o hang.o

COBJ_MODEL=$(COBJ_COMMON) tube.o

COBJ=$(COBJ_MODEL) $(COBJ_COMMON)

CSRC = $(subst .o,.c,$(COBJ))
ASRC = $(subst .o,.S,$(AOBJ))

PROG=pure-usb

TARGETS=$(PROG)-board.bin $(PROG)-model.bin

all: $(TARGETS)
	
#$(AOBJ): %.o : %.S
	#$(CC) $(CFLAGS) -c -o $@ $<
	
#$(COBJ): %.o : %.c
	#$(CC) $(CFLAGS) -c -o $@ $<
	
$(PROG)-board.elf: $(COBJ_BOARD) $(AOBJ)
	$(LD) -T boot.lds -Ttext=$(RAM_BASE) $^ -o $@

$(PROG)-model.elf: $(COBJ_MODEL) $(AOBJ)
	$(LD) -T boot.lds -Ttext=$(RAM_BASE) $^ -o $@

$(TARGETS): %.bin : %.elf
	$(OBJCOPY) -v -O binary $< $@

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

.depend: Makefile $(CSRC) $(ASRC)
	@for f in $(CSRC) $(ASRC); do \
		$(CC) -M $(CFLAGS) $$f >> $@ ; \
	done

-include .depend

