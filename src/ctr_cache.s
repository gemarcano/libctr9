@*******************************************************************************
@* Copyright (C) 2016, 2017 Gabriel Marcano
@*
@* Refer to the COPYING.txt file at the top of the project directory. If that is
@* missing, this file is licensed under the GPL version 2.0 or later.
@*
@******************************************************************************/

.arm

.align 4

.global ctr_cache_clean_and_flush_all, ctr_cache_flush_data_all
.global ctr_cache_flush_data_entry, ctr_cache_data_clean_entry
.global ctr_cache_data_clean_and_flush_entry, ctr_cache_data_clean_index_entry
.global ctr_cache_data_clean_and_flush_index_entry
.global ctr_cache_flush_instruction_all, ctr_cache_flush_instruction_entry
.global ctr_cache_prefetch_instruction_line, ctr_cache_clean_and_flush
.global ctr_cache_clean_data_range, ctr_cache_flush_data_range
.global ctr_cache_flush_instruction_range, ctr_cache_drain_write_buffer

.type ctr_cache_clean_and_flush_all, %function
.type ctr_cache_flush_data_all, %function
.type ctr_cache_flush_data_entry, %function
.type ctr_cache_data_clean_entry, %function
.type ctr_cache_data_clean_and_flush_entry, %function
.type ctr_cache_data_clean_index_entry, %function
.type ctr_cache_data_clean_and_flush_index_entry, %function
.type ctr_cache_flush_instruction_all, %function
.type ctr_cache_flush_instruction_entry, %function
.type ctr_cache_prefetch_instruction_line, %function
.type ctr_cache_clean_and_flush, %function
.type ctr_cache_clean_data_range, %function
.type ctr_cache_flush_data_range, %function
.type ctr_cache_flush_instruction_range, %function
.type ctr_cache_drain_write_buffer, %function

ctr_cache_clean_and_flush_all:
	mov r1, #0 @segment

	1:	@outer_loop
		mov r0, #0 @line
		2:  @inner_loop
			orr r2, r1, r0  @make r2 be the resulting combination of
								  @segment and line

			mcr p15, 0, r2, c7, c14, 2  @Clean and flush the line

			add r0, r0, #0x20  @move line to next line

			@The number of lines depends on the cache size, 0x400 for the
			@3DS data cache since it has 8KB of cache. Refer to the ARM
			@documentation for more details.
			cmp r0, #0x400
			bne 2b  @inner_loop

		add r1, r1, #0x40000000
		cmp r1, #0x0
		bne 1b  @outer_loop

	mov r0, #0
	mcr p15, 0, r0, c7, c5, 0  @Flush instruction cache
	mcr p15, 0, r0, c7, c10, 4  @drain write buffer
	bx lr

ctr_cache_flush_data_all:
	mov r0, #0
	mcr p15, 0, r0, c7, c6, 0
	bx lr

.macro define_entry_function c, segment=1
	bic r0, #0x1F
	mcr p15, 0, r0, c7, \c, \segment
	bx lr
.endm

ctr_cache_flush_data_entry:
	define_entry_function c6

ctr_cache_data_clean_entry:
	define_entry_function c10

ctr_cache_data_clean_and_flush_entry:
	define_entry_function c14

ctr_cache_data_clean_index_entry:
	lsl r0, #30
	bic r1, #0xFFFFFFC0
	orr r0, r0, r1, lsl #5
	define_entry_function c10 2

ctr_cache_data_clean_and_flush_index_entry:
	lsl r0, #30
	bic r1, #0xFFFFFFC0
	orr r0, r0, r1, lsl #5
	define_entry_function c14 2

ctr_cache_flush_instruction_all:
	mov r0, #0
	mcr p15, 0, r0, c7, c5, 0
	bx lr

ctr_cache_flush_instruction_entry:
	mcr p15, 0, r0, c7, c5, 1
	bx lr

ctr_cache_prefetch_instruction_line:
	mcr p15, 0, r0, c7, c13, 1

.macro define_range_function c
	cmp r0, r1
	bhs 2f

	1:
	mcr p15, 0, r0, c7, \c, 1
	add r0, #32
	cmp r0, r1
	ble 1b

	2:
	bx lr
.endm

ctr_cache_clean_and_flush_data_range:
	define_range_function c14

ctr_cache_clean_data_range:
	define_range_function c10

ctr_cache_flush_data_range:
	define_range_function c6

ctr_cache_flush_instruction_range:
	define_range_function c5

ctr_cache_drain_write_buffer:
	mov r0, #0
	mcr p15, 0, r0, c7, c10, 4
	bx lr
