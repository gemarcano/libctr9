/*******************************************************************************
 * Copyright (C) 2016 Gabriel Marcano
 *
 * Refer to the COPYING.txt file at the top of the project directory. If that is
 * missing, this file is licensed under the GPL version 2.0 or later.
 *
 ******************************************************************************/

#include <ctr9/i2c.h>
#include <time.h>
#include <assert.h>

#include <ctr9/ctr_rtc.h>

typedef struct
{
	time_t time;

} ctr_rtc;

static inline uint8_t bcd_to_number(uint8_t bcd)
{
	return (bcd & 0xF) + (bcd >> 4) * 10;
}

static ctr_rtc rtc;

void ctr_rtc_init(void)
{
	rtc.time = 0;
}

ctr_rtc_data ctr_rtc_gettime(void)
{
	ctr_rtc_data data;
	static_assert(sizeof(data) == 7, "mismatch in RTC data size");
	ctr_core_i2c_read(CTR_I2C2, 0x4A, 0x30, (uint8_t*)&data, sizeof(data));
	return data;
}

