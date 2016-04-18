OUTPUT_ARCH(arm)
ENTRY(_entry)
SECTIONS
{
	. = 0x23F00000;
	.text : {
		*(.text.start)
		*(.text)
	}
	.data : {
		*(.data)
	}
	__bss_start = .;
	.bss : {
		*(.bss COMMON)
	}
	__bss_end = .;
	.rodata : {
		*(.rodata)
	}
	. = ALIGN(4);
	__end__ = 0x20400000;
}

