/*******************************************************************************
 * Copyright (C) 2016 Gabriel Marcano
 *
 * Refer to the COPYING.txt file at the top of the project directory. If that is
 * missing, this file is licensed under the GPL version 2.0 or later.
 *
 ******************************************************************************/

/** @file */

#ifndef CTR_RTC_H_
#define CTR_RTC_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**	@brief Represents the RTC data returned by the RTC hardware.
 */
typedef struct
{
	uint8_t seconds, minutes, hours, pad_, day, month, year;
} ctr_rtc_data;

/**	@brief Initializes the data used for RTC access.
 *
 *	Call this function at least once before trying to get the RTC time. It is
 *	safe to call this function multiple times.
 */
void ctr_rtc_init(void);

/**	@brief Returns the data read from the RTC.
 *
 *	The underlying RTC returns data in BCD format, but this function converts
 *	the BCD to binary.
 *
 *	@pre ctr_rtc_init has been called at least once.
 *	@returns The binary representation of the RTC BCD data.
 */
ctr_rtc_data ctr_rtc_gettime(void);

#ifdef __cplusplus
}
#endif

#endif//CTR_RTC_H_

