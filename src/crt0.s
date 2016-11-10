.align 4

.global _start, ctr_libctr9_init

.section .text.start, "x"

_start:
	@ Disable IRQ
	mrs r0, cpsr
	orr r0, r0, #0x80
	msr cpsr_c, r0

	@ Flush caches, make sure to sync memory with what's on the cache before
	@ turning off the MPU
	adr r0, flush_all_caches_offset
	ldr r1, [r0]
	add r0, r1, r0
	blx r0

	@ Disable caches and MPU
	adr r0, disable_mpu_and_caching_offset
	ldr r1, [r0]
	add r0, r1, r0
	blx r0

	@ Flush caches, for good measure. Makes sure there is nothing in the caches
	@ when the MPU is brought back online.
	adr r0, flush_all_caches_offset
	ldr r1, [r0]
	add r0, r1, r0
	blx r0

	@ Switch to system mode
	mrs r0, cpsr
	orr r1, r0, #0x1F
	msr cpsr_c, r1

	@ Change the stack pointer
	ldr sp, =_stack

	@ Enable both ITCM and DTCM
	mrc p15, 0, r0, c1, c0, 0
	orr r0, r0, #(1<<18)
	bic r0, r0, #(1<<17)
	orr r0, r0, #(1<<16)
	mcr p15, 0, r0, c1, c0, 0

	@ Configure ITCM to have a size of 64MB
	mrc p15, 0, r0, c9, c1, 1
	bic r0, #0b111110
	orr r0, #0b100010
	mcr p15, 0, r0, c9, c1, 1

	@ Configure DTCM to have a size of 16kB
	@ Set the base address to 0x30000000
	mrc p15, 0, r0, c9, c1, 0
	bic r0, #0b111110
	orr r0, #0b001010
	ldr r1, =0xFFFFF000
	bic r0, r1
	ldr r1, =0x30000000
	orr r0, r1
	mcr p15, 0, r0, c9, c1, 0

	@ clear bss
	adr r0, clear_bss_offset
	ldr r1, [r0]
	add r0, r1, r0
	blx r0

	@ Give read/write access to all the memory regions
	ldr r0, =0x33333333
	mcr p15, 0, r0, c5, c0, 2 @ write data access
	mcr p15, 0, r0, c5, c0, 3 @ write instruction access

	@ Set MPU permissions and cache settings
	ldr r0, =0xFFFF001F @ ffff0000 64k   | bootrom
	ldr r1, =0x3000801B @ 30000000 16k   | dtcm
	ldr r2, =0x00000035 @ 00000000 128MB | itcm
	ldr r3, =0x08000029 @ 08000000 2M	| arm9 mem
	ldr r4, =0x10000029 @ 10000000 2M	| io mem
	ldr r5, =0x20000037 @ 20000000 256M  | fcram
	ldr r6, =0x1FF00027 @ 1FF00000 1M	| DSP memory
	ldr r7, =0x1800002D @ 18000000 8M	| vram
	mcr p15, 0, r0, c6, c0, 0
	mcr p15, 0, r1, c6, c1, 0
	mcr p15, 0, r2, c6, c2, 0
	mcr p15, 0, r3, c6, c3, 0
	mcr p15, 0, r4, c6, c4, 0
	mcr p15, 0, r5, c6, c5, 0
	mcr p15, 0, r6, c6, c6, 0
	mcr p15, 0, r7, c6, c7, 0
	mov r0, #0b10101001		@ unprot | arm9 | fcram | vram
	mcr p15, 0, r0, c2, c0, 0  @ data cacheable
	mcr p15, 0, r0, c2, c0, 1  @ instruction cacheable
	mcr p15, 0, r0, c3, c0, 0  @ data bufferable

	@ Enable caches and MPU
	adr r0, enable_mpu_and_caching_offset
	ldr r1, [r0]
	add r0, r1, r0
	blx r0

	@ Fix mounting of SDMC
	ldr r0, =0x10000020
	mov r1, #0x340
	str r1, [r0]

	@call libc initialization routines
	adr r0, __libc_init_array_offset
	ldr r1, [r0]
	add r0, r1, r0
	blx r0

	adr r0, _fini_offset
	ldr r1, [r0]
	add r0, r1, r0
	adr r1, atexit_offset
	ldr r2, [r1]
	add r1, r2, r1
	blx r1

	adr r0, _init_offset
	ldr r1, [r0]
	add r0, r1, r0
	blx r0

	@Initialize libctr9 (weak symbol)
	adr r0, ctr_libctr9_init_offset
	ldr r1, [r0]
	add r0, r1, r0
	blx r0

	@ Make sure to pass in argc as 0 and argv as NULL
	mov r0, #0
	mov r1, #0

	@ Launch main(0, NULL)
	adr r2, main_offset
	ldr r3, [r2]
	add r2, r3, r2
	blx r2
	bx lr

	@if we do return, make sure to call exit functions.
	adr r1, exit_offset
	ldr r2, [r1]
	add r1, r2, r1
	blx r1

	b . @die if we return, just forcibly hang

disable_mpu_and_caching_offset:
.word disable_mpu_and_caching-.

enable_mpu_and_caching_offset:
.word enable_mpu_and_caching-.

flush_all_caches_offset:
.word flush_all_caches-.

__bss_start_offset:
.word __bss_start__-.

__bss_end_offset:
.word __bss_end__-.

clear_bss_offset:
.word clear_bss-.

__libc_init_array_offset:
.word __libc_init_array-.

_fini_offset:
.word _fini-.

atexit_offset:
.word atexit-.

_init_offset:
.word _init-.

ctr_libctr9_init_offset:
.word ctr_libctr9_init-.

main_offset:
.word main-.

exit_offset:
.word exit-.

clear_bss:
	@clear bss
	adr r0, __bss_start_offset
	ldr r1, [r0]
	add r0, r1, r0

	adr r1, __bss_end_offset
	ldr r2, [r1]
	add r1, r2, r1
	mov r2, #0
	.Lclear_bss_loop:
		cmp r0, r1
		beq .Lclear_bss_loop_done
		str r2, [r0], #4
		b .Lclear_bss_loop
	.Lclear_bss_loop_done:
	blx lr

disable_mpu_and_caching:
	@ Disable caches and MPU
	mrc p15, 0, r0, c1, c0, 0  @ read control register
	bic r0, r0, #(1<<12)	   @ - instruction cache enable
	bic r0, r0, #(1<<2)		@ - data cache enable
	bic r0, r0, #(1<<0)		@ - mpu enable
	mcr p15, 0, r0, c1, c0, 0  @ write control register

	bx lr

enable_mpu_and_caching:
	@ Enable caches and MPU
	mrc p15, 0, r0, c1, c0, 0  @ read control register
	orr r0, r0, #(1<<12)	   @ - instruction cache enable
	orr r0, r0, #(1<<2)		@ - data cache enable
	orr r0, r0, #(1<<0)		@ - mpu enable
	mcr p15, 0, r0, c1, c0, 0  @ write control register

	bx lr

flush_all_caches:
	@ flush instruction cache, it's not flushed by Nintendo's function
	mov r0, #0
	mcr p15, 0, r0, c7, c5, 0

	@ Nintendo's function uses r0-r2, r12, all registers that don't need
	@ to be saved, just be aware that they are changed
	@ use Nintendo's bx lr to return
	ldr r0, =0xFFFF0830 @ Nintendo's flush function in unprot. bootrom
	bx r0

