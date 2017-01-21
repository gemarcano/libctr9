/*******************************************************************************
 * Copyright (C) 2016, 2017 Gabriel Marcano
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

/**	@brief Represents the clock internal data.
 */
typedef struct
{
	uint64_t count;
	ctr_timer timer;
} ctr_system_clock;

/**	@brief Represents a detailed time structure used by the system clock.
 */
typedef struct
{
	int64_t seconds;
	int32_t  nanoseconds;
} ctr_clock_time;

/**	@brief Initializes a clock, taking control of the specified timer.
 *
 *	Once initialized, the system clock overrides whatever settings the
 *	timer was set to. In order for the clock to work, global IRQs must
 *	be enabled.
 *
 *	@param[out] clock Pointer to clock data structure to initialize.
 *	@param[in[ timer Timer to take control over.
 *
 */
void ctr_system_clock_initialize(ctr_system_clock *clock, ctr_timer timer);

/**	@brief Returns the number of milliseconds that have passed since the clock
 *		started.
 *
 *	@param[in] clock Pointer to the clock to query.
 *
 *	@returns The number of milliseconds that have passed since the clock
 *		started.
 */
uint64_t ctr_system_clock_get_ms(ctr_system_clock *clock);

/**	@brief Returns a detailed time structure representing the amount of time
 *		that has passed since the clock was started.
 *
 *	@param[in] clock Pointer to the clock to query.
 *
 *	@returns A detailed time structure representing the amount of time that
 *		has passed since the clock was started.
 */
ctr_clock_time ctr_system_clock_get_time(ctr_system_clock *clock);

#ifdef __cplusplus
}
#endif

#endif//CTR_SYSTEM_CLOCK_H_

