@*******************************************************************************
@* Copyright (C) 2016 Gabriel Marcano
@*
@* Refer to the COPYING.txt file at the top of the project directory. If that is
@* missing, this file is licensed under the GPL version 2.0 or later.
@*
@******************************************************************************/

.arm

.align 4

.global ctr_interrupt_reset_veneer, ctr_interrupt_undef_veneer
.global ctr_interrupt_swi_veneer, ctr_interrupt_preabrt_veneer
.global ctr_interrupt_databrt_veneer, ctr_interrupt_irq_veneer
.global ctr_interrupt_fiq_veneer

.type ctr_interrupt_reset_veneer, %function
.type ctr_interrupt_undef_veneer, %function
.type ctr_interrupt_swi_veneer, %function
.type ctr_interrupt_preabrt_veneer, %function
.type ctr_interrupt_databrt_veneer, %function
.type ctr_interrupt_irq_veneer, %function
.type ctr_interrupt_irq_veneer, %function
.type ctr_interrupt_fiq_veneer, %function

.extern ctr_interrupt_handlers

.macro CTR_INTERRUPT_VENEER table_offset=0
	push {r0-r12,r14} @push "normal" registers

	@switch to previous mode to grab stack pointer and LR, then switch back
	mrs r1, cpsr
	mrs r0, spsr
	mov r3, r0
	orr r3, #0xC0 @make sure interrupts are disabled in mode to be switched to
	bic r3, #0x20 @make sure the status flag T matches ARM mode

	msr cpsr_c, r3
	mov r3, r13
	mov r4, r14
	msr cpsr_c, r1

	@push "special" registers, placing them in the front of the array to be
	@passed
	push {r0, r3, r4, r14}

	adr r1, ctr_interrupt_handlers_location
	ldr r2, [r1]
	add r1, r1, r2
	ldr r2, [r1, #\table_offset]
	mov r0, sp

	@Parameters: r0 - pointer to array on stack:
	@    cpsr, sp, lr, return, r0-r12
	blx r2

	@now determine the mode we were in previously
	@if Thumb, make sure LR is updated
	pop {r0, r3, r4, r14}
	tst r0, #0x20
	beq 1f
	orr r14, r14, #1

	1:
	str r14, [sp, #0x34]
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

ctr_interrupt_handlers_location:
.word ctr_interrupt_handlers - .

