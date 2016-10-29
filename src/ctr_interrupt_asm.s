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

@FIXME this code is not generally position independent

prev_sp:
.word 0
prev_lr:
.word 0
tmp_stack_end:
.word 0
.word 0
.word 0
tmp_stack:

.macro CTR_INTERRUPT_VENEER table_offset=0
	@temporary stack...
	ldr sp, =tmp_stack
	push {r0, r1, r2}

	@switch to previous mode to previous stack pointer, then switch back
	@previous stack pointer will be used for current mode
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
	mov r0, sp
	mov r2, lr
	@switch back, to finish setting up the stack for the interrupt
	msr cpsr_c, r1

	@store the retreived sp and lr and restore r0-r2 original state
	ldr r1, =prev_sp
	str r0, [r1]

	ldr r1, =prev_lr
	str r2, [r1]

	pop {r0, r1, r2}

	@finally finish setting up the stack
	ldr sp, =prev_sp
	ldr sp, [sp]

	push {r0-r12,r14} @push "normal" registers

	@load the saved sp and lr to push onto the stack to pass to the registered
	@interrupt handler
	ldr r0, =prev_sp
	ldr r3, [r0]

	ldr r0, =prev_lr
	ldr r4, [r0]

	mrs r0, spsr

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

