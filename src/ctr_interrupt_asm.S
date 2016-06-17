.arm

.align 4

.global ctr_interrupt_reset_veneer, ctr_interrupt_undef_veneer, ctr_interrupt_swi_veneer, ctr_interrupt_preabrt_veneer, ctr_interrupt_databrt_veneer, ctr_interrupt_irq_veneer, ctr_interrupt_fiq_veneer

.extern ctr_interrupt_handlers

.macro scribble
	push {r0, r1, r2}
    ldr r0, =0x18300000
    ldr r1, =0xFFFFFFFF
    add r2, r0, #0x2000

    1:
    str r1, [r0], #4
    cmp r0, r2
    bls 1b
    pop {r0, r1, r2}
    blx lr
.endm

ctr_interrupt_handlers_location:
.word ctr_interrupt_handlers - .

.macro CTR_INTERRUPT_VENEER table_offset=0
	push {r0-r12, r14}
	mrs r0, spsr
	push {r0}
	
	adr r1, ctr_interrupt_handlers_location
	ldr r2, [r1]
	add r1, r1, r2
	ldr r2, [r1, #\table_offset]
	mov r0, sp
	blx r2

	@now determine the mode we were in previously
	pop {r0}
	tst r0, #0x40
	beq 1f
	ldr r0, [sp, #0x34]
	orr r0, r0, #1

	1:
	ldmfd sp!, {r0-r12, pc}^
.endm


ctr_interrupt_reset_veneer:
	CTR_INTERRUPT_VENEER 0

ctr_interrupt_undef_veneer:
	CTR_INTERRUPT_VENEER 4

ctr_interrupt_swi_veneer:
	CTR_INTERRUPT_VENEER 8

ctr_interrupt_preabrt_veneer:
	CTR_INTERRUPT_VENEER 12

ctr_interrupt_databrt_veneer:
	CTR_INTERRUPT_VENEER 16

ctr_interrupt_irq_veneer:
	CTR_INTERRUPT_VENEER 20

ctr_interrupt_fiq_veneer:
	CTR_INTERRUPT_VENEER 24

