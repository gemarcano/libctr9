#ifndef CTR_INTERRUPT_H_
#define CTR_INTERRUPT_H_

typedef enum
{   
	CTR_INTERRUPT_RESET,
	CTR_INTERRUPT_UNDEF,
	CTR_INTERRUPT_SWI,
	CTR_INTERRUPT_PREABRT,
	CTR_INTERRUPT_DATABRT,
	CTR_INTERRUPT_IRQ,
	CTR_INTERRUPT_FIQ
} ctr_interrupt_enum;

void ctr_interrupt_global_disable(void);
void ctr_interrupt_global_enable(void);
void ctr_interrupt_prepare(void);
void ctr_interrupt_set(ctr_interrupt_enum interrupt_type, void (*interrupt)(void));

#endif//CTR_INTERRUPT_H_

