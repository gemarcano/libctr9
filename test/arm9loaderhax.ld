OUTPUT_ARCH(arm)
ENTRY(_entry)
SECTIONS
{
	/* . = 0x23F00000; */
	. = 0;
	.text : {
		*(.text.start)
		*(.text)
		. = ALIGN(4);
	}
	.data : {
		*(.data)
		. = ALIGN(4);
	}
	__bss_start = .;
	.bss : {
		*(.bss COMMON)
		. = ALIGN(4);
	}
	__bss_end = .;
	.rodata : {
		*(.rodata)
		. = ALIGN(4);
	}

	.plt :
	{
		*(.plt)
	}

	__got_start = .;
	.got :
	{
		*(got.plt)
		*(.got)
	}
	__got_end = .;

	__end__ = . ;/*0x20400000; */
}

