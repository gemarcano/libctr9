/*******************************************************************************
 * Copyright (C) 2016 Gabriel Marcano
 *
 * Refer to the COPYING.txt file at the top of the project directory. If that is
 * missing, this file is licensed under the GPL version 2.0 or later.
 *
 ******************************************************************************/

#include <ctr9/ctr_timer.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdatomic.h>

#define CTR_TIMER_REGISTER ((uint16_t(*)[2])(CTR_TIMER_REG_BASE))

uint32_t ctr_timer_get_combined_value(ctr_timer base_timer)
{
	if (base_timer > 2)
		return 0;
	
	atomic_uint_fast64_t raw = *(volatile atomic_uint_fast64_t*)(CTR_TIMER_REG_BASE + base_timer);
	uint32_t result = ((raw >> 32) & 0xFFFF0000) | (raw & 0xFFFF);
	return result;
}

uint16_t ctr_timer_get_value(ctr_timer timer)
{
	if (timer > 3)
		return 0;
	
	return CTR_TIMER_REGISTER[timer][0];
}

bool ctr_timer_get_irq_enabled(ctr_timer timer)
{
	if (timer > 3)
		return false;
	return CTR_TIMER_REGISTER[timer][1] & 0x0040;
}

void ctr_timer_set_irq_enabled(ctr_timer timer, bool enable)
{
	if (timer > 3)
		return;
	uint16_t ctrl = CTR_TIMER_REGISTER[timer][1];
	ctrl &= ~0x0040u;
	ctrl |= enable << 6;
	CTR_TIMER_REGISTER[timer][1] = ctrl;
}

bool ctr_timer_get_state(ctr_timer timer)
{
	if (timer > 3)
		return false;
	return CTR_TIMER_REGISTER[timer][2] & 0x0080;
}

void ctr_timer_set_state(ctr_timer timer, bool enable)
{
	if (timer > 3)
		return;
	uint16_t ctrl = CTR_TIMER_REGISTER[timer][1];
	ctrl &= ~0x0080u;
	ctrl |= enable << 7;
	CTR_TIMER_REGISTER[timer][1] = ctrl;
}

bool ctr_timer_get_count_up(ctr_timer timer)
{
	if (timer > 3)
		return false;
	return CTR_TIMER_REGISTER[timer][1] & 0x0004;
}

void ctr_timer_set_count_up(ctr_timer timer, bool enable)
{
	if (timer > 3)
		return;
	uint16_t ctrl = CTR_TIMER_REGISTER[timer][1];
	ctrl &= ~0x0004u;
	ctrl |= enable << 2;
	CTR_TIMER_REGISTER[timer][1] = ctrl;
}

void ctr_timer_set_prescaler(ctr_timer timer, ctr_timer_prescaler setting)
{
	if (timer > 3)
		return;

	uint16_t ctrl = CTR_TIMER_REGISTER[timer][1];
	ctrl &= ~0x3;
	ctrl |= setting & 0x3;
	CTR_TIMER_REGISTER[timer][1] = ctrl;
}

ctr_timer_prescaler ctr_timer_get_prescaler(ctr_timer timer)
{
	if (timer > 3)
		return CTR_TIMER_DIV_UNKNOWN;

	return CTR_TIMER_REGISTER[timer][1] & 0x3;
}

uint32_t ctr_timer_get_effective_frequency(ctr_timer timer)
{
	uint32_t divider;
	switch (ctr_timer_get_prescaler(timer))
	{
		case CTR_TIMER_DIV1:
			divider = 1;
			break;
		case CTR_TIMER_DIV64:
			divider = 64;
			break;
		case CTR_TIMER_DIV256:
			divider = 256;
			break;
		case CTR_TIMER_DIV1024:
			divider = 1024;
			break;
		case CTR_TIMER_DIV_UNKNOWN:
		default:
			return 0;
	}

	return CTR_TIMER_FREQ / divider;
}

