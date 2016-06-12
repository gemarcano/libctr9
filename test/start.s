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
	sub r3, pc, #24
	ldr r0, =__got_start
	ldr r1, =__got_end

	add r0, r0, r3
	add r1, r1, r3

	got_init:
	cmp r0, r1
	beq gotinit_done
	ldr r2, [r0]
	add r2, r2, r3
	str r2, [r0], #4
	b got_init
	gotinit_done:

	@ data.rel.ro
	ldr r0, =__data_rel_ro_start
	ldr r1, =__data_rel_ro_end

	add r0, r0, r3
	add r1, r1, r3

	data_rel_ro_init:
	cmp r0, r1
	beq data_rel_ro_done
	ldr r2, [r0]
	add r2, r2, r3
	str r2, [r0], #4
	b data_rel_ro_init
	data_rel_ro_done:
	
	@ data.rel.ro.local
	ldr r0, =__data_rel_ro_local_start
	ldr r1, =__data_rel_ro_local_end

	add r0, r0, r3
	add r1, r1, r3

	data_rel_ro_local_init:
	cmp r0, r1
	beq data_rel_ro_local_done
	ldr r2, [r0]
	add r2, r2, r3
	str r2, [r0], #4
	b data_rel_ro_local_init
	data_rel_ro_local_done:

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

	push {r3}
	
    @ Give read/write access to all the memory regions
    ldr r0, =0x33333333
    mcr p15, 0, r0, c5, c0, 2 @ write data access
    mcr p15, 0, r0, c5, c0, 3 @ write instruction access

    @ Set MPU permissions and cache settings
    ldr r0, =0xFFFF001D @ ffff0000 32k | bootrom unprotected
    ldr r1, =0x3000801B @ fff00000 16k | dtcm
	ldr r2, =0x01FF801D @ 01ff8000 32k | itcm
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
    mov r0, #0xA5 @ Fixes payloads which don't like FCRAM as "data bufferable"
    mcr p15, 0, r0, c3, c0, 0  @ data bufferable
    
	@ Flush caches
    mov r0, #0
    mcr p15, 0, r0, c7, c5, 0  @ flush I-cache
    mcr p15, 0, r0, c7, c6, 0  @ flush D-cache
    mcr p15, 0, r0, c7, c10, 4 @ drain write buffer

	@ Enable caches and turn on MPU
    mrc p15, 0, r0, c1, c0, 0  @ read control register
    orr r0, r0, #(1<<18) @ - itcm enable
	bic r0, r0, #(1<<19)
	orr r0, r0, #(1<<12)       @ - instruction cache enable
    orr r0, r0, #(1<<2)        @ - data cache enable
    orr r0, r0, #(1<<0)        @ - mpu enable
    mcr p15, 0, r0, c1, c0, 0  @ write control register

    @ Fix mounting of SDMC
    ldr r0, =0x10000020
    mov r1, #0x340
    str r1, [r0]

	pop {r3}
	ldr r0, =main
	add r0, r0, r3

    blx r0
	bx lr
