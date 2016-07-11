/*******************************************************************************
 * Copyright (C) 2016 Gabriel Marcano
 *
 * Refer to the COPYING.txt file at the top of the project directory. If that is
 * missing, this file is licensed under the GPL version 2.0 or later.
 *
 ******************************************************************************/

/** @file */

#ifndef CTR_TIMER_H_
#define CTR_TIMER_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

//TIMER Register base address
#define CTR_TIMER_REG_BASE (0x10003000)

//Interestingly enough, this is about half of the ARM9 CPU frequency, and twice
//the timer frequency of the NDS. I'm willing to wager the real number of this
//frequency is exactly half of the CPU frequency. The question is, out of the
//numbers we have, which is the more accurate? The CPU one or the one for this
//timer?
#define CTR_TIMER_FREQ (67027964u)

typedef enum
{
	CTR_TIMER0 = 0,
	CTR_TIMER1 = 1,
	CTR_TIMER2 = 2,
	CTR_TIMER3 = 3
} ctr_timer;

uint32_t ctr_timer_get_combined_value(ctr_timer base_timer);
uint16_t ctr_timer_get_value(ctr_timer timer);

bool ctr_timer_get_irq_enabled(ctr_timer timer);
void ctr_timer_set_irq_enabled(ctr_timer timer, bool enable);

bool ctr_timer_get_state(ctr_timer timer);
void ctr_timer_set_state(ctr_timer, bool enable);

bool ctr_timer_get_count_up(ctr_timer timer);
void ctr_timer_set_count_up(ctr_timer timer, bool enable);

typedef enum
{
	CTR_TIMER_DIV1 = 0,
	CTR_TIMER_DIV64 = 1,
	CTR_TIMER_DIV256 = 2,
	CTR_TIMER_DIV1024 = 3,
	CTR_TIMER_DIV_UNKNOWN
} ctr_timer_prescaler;

void ctr_timer_set_prescaler(ctr_timer timer, ctr_timer_prescaler setting);
ctr_timer_prescaler ctr_timer_get_prescaler(ctr_timer timer);

uint32_t ctr_timer_get_effective_frequency(ctr_timer timer);

#ifdef __cplusplus
}
#endif

#endif//CTR_TIMER_H_

