@*******************************************************************************
@* Copyright (C) 2016 Gabriel Marcano
@*
@* Refer to the COPYING.txt file at the top of the project directory. If that is
@* missing, this file is licensed under the GPL version 2.0 or later.
@*
@******************************************************************************/

.arm
.align 4

.type ctr_memory_get_itcm_register_, %function
.type ctr_memory_get_dtcm_register_, %function
.type ctr_memory_set_itcm_register_, %function
.type ctr_memory_set_dtcm_register_, %function
.type ctr_memory_itcm_state_, %function
.type ctr_memory_dtcm_state_, %function

.global ctr_memory_get_itcm_register, ctr_memory_get_dtcm_register
.global ctr_memory_set_itcm_register, ctr_memory_set_itcm_register
.global ctr_memory_itcm_state_, ctr_memory_dtcm_state_
.global ctr_memory_enable_itcm_, ctr_memory_disable_itcm_
.global ctr_memory_enable_dtcm_, ctr_memory_disable_dtcm_

ctr_memory_get_itcm_register_:
	mrc p15, 0, r0, c9, c1, 0
	bx lr
	
ctr_memory_get_dtcm_register_:
	mrc p15, 0, r0, c9, c1, 1
	bx lr

ctr_memory_set_itcm_register_:
	mcr p15, 0, r0,  c9, c1, 0
	bx lr
	
ctr_memory_set_dtcm_register_:
	mcr p15, 0, r0,  c9, c1, 1
	bx lr

ctr_memory_itcm_state_:
	mrc p15, 0, r0, c1, c0, 0
	and r0, #0x40000
	bx lr

ctr_memory_dtcm_state_:
	mrc p15, 0, r0, c1, c0, 0
	and r0, #0x10000
	bx lr

ctr_memory_enable_itcm_:
	mrc p15, 0, r0, c1, c0, 0
	orr r0, #0x40000
	mcr p15, 0, r0, c1, c0, 0
	bx lr

ctr_memory_disable_itcm_:
	mrc p15, 0, r0, c1, c0, 0
	bic r0, #0x40000
	mcr p15, 0, r0, c1, c0, 0
	bx lr

ctr_memory_enable_dtcm_:
	mrc p15, 0, r0, c1, c0, 0
	orr r0, #0x10000
	mcr p15, 0, r0, c1, c0, 0
	bx lr

ctr_memory_disable_dtcm_:
	mrc p15, 0, r0, c1, c0, 0
	bic r0, #0x10000
	mcr p15, 0, r0, c1, c0, 0
	bx lr

