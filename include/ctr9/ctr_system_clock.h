/*******************************************************************************
 * Copyright (C) 2016 Gabriel Marcano
 *
 * Refer to the COPYING.txt file at the top of the project directory. If that is
 * missing, this file is licensed under the GPL version 2.0 or later.
 *
 ******************************************************************************/

/** @file */

#ifndef CTR_SYSTEM_CLOCK_H_
#define CTR_SYSTEM_CLOCK_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
	uint64_t count;
	ctr_timer timer;
} ctr_system_clock;

void ctr_system_clock_initialize(ctr_system_clock *clock, ctr_timer timer);

typedef struct
{
	int64_t seconds;
	int32_t  nanoseconds;
} ctr_clock_time;

uint64_t ctr_system_clock_get_ms(ctr_system_clock *clock);

ctr_clock_time ctr_system_clock_get_time(ctr_system_clock *clock);

#ifdef __cplusplus
}
#endif

#endif//CTR_SYSTEM_CLOCK_H_

