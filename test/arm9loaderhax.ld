OUTPUT_ARCH(arm)
ENTRY(_entry)
SECTIONS
{
	. = 0x23F00000;
	/* . = 0; */
	.text : {
		*(.text.start)
		*(.text)
		. = ALIGN(4);
	}
	.data : {
		*(.data)
				. = ALIGN(4);
	}

	.data.rel.ro.local : {
		__data_rel_ro_local_start = .;
		*(.data.rel.ro)
		. = ALIGN(4);
	}
	__data_rel_ro_local_end = .;

	.data.rel.ro : {
		__data_rel_ro_start = .;
		*(.data.rel.ro)
		. = ALIGN(4);
	}
	__data_rel_ro_end = .;

	.bss ALIGN(4) : {
		__bss_start = .;
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

	.got ALIGN(4) :
	{
		__got_start = .;
		*(got.plt)
		*(.got)
		. = ALIGN(4);
	}
	__got_end = .;

	__end__ = . ;/*0x20400000; */
}

