OUTPUT_ARCH(ARM)
ENTRY(_start)
SECTIONS
{
    . = 0;
	. = ALIGN(4);
    .text : {
		start.o (.text)
		*(.text)
    }

    .rodata	: {
		*(.rodata.*)
    }

	. = ALIGN(4);
	.data : {
		*(.data)
	}

    .= 0x140000;
    .bss : {
		*(.bss)
    }

	/DISCARD/ : { *(.dynstr*) }
	/DISCARD/ : { *(.dynamic*) }
	/DISCARD/ : { *(.plt*) }
	/DISCARD/ : { *(.interp*) }
	/DISCARD/ : { *(.gnu*) }
}
