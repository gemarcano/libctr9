@*******************************************************************************
@* Copyright (C) 2016, 2017 Gabriel Marcano
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

.extern ctr_interrupt_handlers, ctr_interrupt_data

@FIXME this code is not generally position independent

.macro CTR_INTERRUPT_VENEER table_offset=0
	@Exceptions stack. Currently we don't support nested exceptions
	ldr sp, =_istack
	push {r0-r12,r14} @push "normal" registers

	@switch to previous mode to get previous sp and lr, then switch back
	mrs r1, cpsr @interrupt mode
	mrs r0, spsr @previous mode
	orr r0, #0xC0 @make sure interrupts are disabled in mode to be switched to
	bic r0, #0x20 @make sure the status flag T matches ARM mode

	@if the previous mode is USER, we don't want to switch to user but to system,
	@since system is privileged and shares the same registers as user
	and r2, r0, #0x1F @extract the mode bits from r0
	cmp r2, #0x10 @check if it's user mode
	bne 1f
	orr r0, #0x1F @set the mode to system if user mode was detected
	1:

	@change mode
	msr cpsr_c, r0
	mov r2, sp
	mov r3, lr
	@switch back
	msr cpsr_c, r1

	mrs r0, spsr

	@push "special" registers, placing them in the front of the array to be
	@passed
	push {r0, r2, r3, r14}

	adr r1, ctr_interrupt_handlers_location
	ldr r2, [r1]
	add r1, r1, r2
	ldr r2, [r1, #\table_offset]
	mov r0, sp

	adr r1, ctr_interrupt_data_location
	ldr r3, [r1]
	add r1, r3
	ldr r1, [r1, #\table_offset]

	@Parameters: r0 - pointer to array on stack:
	@    cpsr, sp, lr, return, r0-r12
	blx r2

	@now determine the mode we were in previously
	@if Thumb, make sure LR is updated
	pop {r0, r3, r4, r14}
	tst r0, #0x20
	beq 3f
	orr r14, r14, #1

	3:
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

ctr_interrupt_data_location:
.word ctr_interrupt_data - .

