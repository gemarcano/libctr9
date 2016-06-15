#.section .text.start, "x"
.align 4

.global _entry, scribble_screen

scribble_screen:
	push {r0, r1, r2}
	ldr r0, =0x18300000
	ldr r1, =0xFFFFFFFF
	add r2, r0, #0x2000

	draw_screen_loop:
	str r1, [r0], #4
	cmp r0, r2
	bls draw_screen_loop
	pop {r0, r1, r2}
	blx lr

.section .text.start, "x"

_entry:
	@ Disable IRQ
	mrs r0, cpsr
	orr r0, r0, #0x80
	msr cpsr_c, r0
	
	@ Change the stack pointer
	ldr sp, =0x27F00000

	@ initialize GOT
	sub r9, pc, #24
	
	@ make sure ITCM is accessible
	mrc p15, 0, r0, c1, c0, 0
	orr r0, r0, #(1<<18)
	mcr p15, 0, r0, c1, c0, 0

	@ Disable caches and MPU
	ldr r0, =disable_mpu_and_caching
	add r0, r0, r9
	blx r0

	@ Flush caches
	ldr r0, =flush_all_caches
	add r0, r0, r9
	blx r0

	ldr r0, =__got_start
	ldr r1, =__got_end
	mov r2, r9

	ldr r3, =relocate_section
	add r3, r3, r9
	blx r3

	@ data.rel.ro
	ldr r0, =__data_rel_ro_start
	ldr r1, =__data_rel_ro_end
	mov r2, r9

	ldr r3, =relocate_section
	add r3, r3, r9
	blx r3

	@ data.rel.ro.local
	ldr r0, =__data_rel_ro_local_start
	ldr r1, =__data_rel_ro_local_end
	mov r2, r9

	ldr r3, =relocate_section
	add r3, r3, r9
	blx r3

	@clear bss
	ldr r0, =__bss_start
	ldr r1, =__bss_end
	add r0, r0, r3
	add r1, r1, r3
	mov r2, #0
	clear_bss_loop:
		cmp r0, r1
		beq clear_bss_loop_done
		str r2, [r0], #4
		b clear_bss_loop
	clear_bss_loop_done:

	@ Give read/write access to all the memory regions
	ldr r0, =0x33333333
	mcr p15, 0, r0, c5, c0, 2 @ write data access
	mcr p15, 0, r0, c5, c0, 3 @ write instruction access

	@ Set MPU permissions and cache settings
	ldr r0, =0xFFFF001D @ ffff0000 32k | bootrom unprotected
	ldr r1, =0x3000801B @ fff00000 16k | dtcm
	#ldr r2, =0x01FF801D @ 01ff8000 32k | itcm
	ldr r2, =0x00000035 @ 08000000 128MB | itcm
	ldr r3, =0x08000029 @ 08000000 2M  | arm9 mem
	ldr r4, =0x10000029 @ 10000000 2M  | io mem
	ldr r5, =0x20000037 @ 20000000 256M| fcram
	ldr r6, =0x1FF00027 @ 1FF00000 1M
	ldr r7, =0x1800002D @ 18000000 8M
	mcr p15, 0, r0, c6, c0, 0
	mcr p15, 0, r1, c6, c1, 0
	mcr p15, 0, r2, c6, c2, 0
	mcr p15, 0, r3, c6, c3, 0
	mcr p15, 0, r4, c6, c4, 0
	mcr p15, 0, r5, c6, c5, 0
	mcr p15, 0, r6, c6, c6, 0
	mcr p15, 0, r7, c6, c7, 0
	mov r0, #0xA5
	mcr p15, 0, r0, c2, c0, 0  @ data cacheable
	mcr p15, 0, r0, c2, c0, 1  @ instruction cacheable
	mcr p15, 0, r0, c3, c0, 0  @ data bufferable
	
	@ Enable caches and MPU
	ldr r0, =enable_mpu_and_caching
	add r0, r0, r9
	blx r0

	@ Fix mounting of SDMC
	ldr r0, =0x10000020
	mov r1, #0x340
	str r1, [r0]


	ldr r4, =main
	add r4, r4, r9
	blx r4
	bx lr

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

@ r0 - region start
@ r1 - region end
@ r2 - relocation base (usually starting PC address)
relocate_section:
	add r0, r0, r2
	add r1, r1, r2

	.Lreloc_init:
	cmp r0, r1
	beq .Lrelocinit_done
	ldr r3, [r0]
	add r3, r2, r3
	str r3, [r0], #4
	b .Lreloc_init
	.Lrelocinit_done:

	bx lr

flush_all_caches:
	push {lr}
	@ Flush caches
	ldr r0, =0xFFFF0830 @ Nintendo's flush function in unprot. bootrom
	blx r0

	@ flush instruction cache, it's not flushed by Nintendo's function
	mov r0, #0
	mcr p15, 0, r0, c7, c5, 0

	pop {lr}
	bx lr



