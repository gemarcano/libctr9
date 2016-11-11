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

/**	@brief Enums that represent the four available timers.
 */
typedef enum
{
	CTR_TIMER0 = 0,
	CTR_TIMER1 = 1,
	CTR_TIMER2 = 2,
	CTR_TIMER3 = 3
} ctr_timer;

/**	@brief Sets the starting value for the given timer to the value specified.
 *
 *	@param[in] timer Timer to set its initial value.
 *	@param[in] value Value to set the timer counter to.
 *
 *	@post The timer has had its counter set to the value given.
 */
void ctr_timer_set_value(ctr_timer timer, uint16_t value);

/**	@brief Returns the current count value for the given timer.
 *
 *	@param[in] timer Timer to query its counter value.
 *
 *	@returns The current count value for the given timer.
 */
uint16_t ctr_timer_get_value(ctr_timer timer);

/**	@brief Enables the timer IRQ for the given timer.
 *
 *	@param[in] timer The timer that will have its IRQ enabled.
 *
 *	@post The IRQ for the given timer will be enabled.
 */
void ctr_timer_enable_irq(ctr_timer timer);

/**	@brief Disables the timer IRQ for the given timer.
 *
 *	@param[in] timer The timer that will have its IRQ disabled.
 *
 *	@post The IRQ for the given timer will be disabled.
 */
void ctr_timer_disable_irq(ctr_timer timer);

/**	@brief Returns the state of the IRQ for the given timer.
 *
 *	@returns True if the IRQ is enabled, false otherwise.
 */
bool ctr_timer_get_irq_state(ctr_timer timer);

/**	@brief Returns the start state of the given timer.
 *
 *	@param[in] timer Timer to request the start state of.
 *
 *	@return True if the timer is started, false otherwise.
 */
bool ctr_timer_get_state(ctr_timer timer);

/**	@brief Starts the given timer.
 *
 *	@param[in] timer Timer to start.
 *
 *	@post Timer has been started.
 */
void ctr_timer_enable(ctr_timer);

/**	@brief Stops the given timer.
 *
 *	@param[in] timer Timer to stop.
 *
 *	@post Timer has been stopped.
 */
void ctr_timer_disable(ctr_timer);

/**	Returns the state of the timer's up count flag.
 *
 *	3DS timers allow for their own count value to be incremented when the
 *	previous timer overflows by enabling the count up flag. This function
 *	returns true if this flag is enabled, false otherwise.
 *
 *	@param[in] timer Timer to query for its count up flag state.
 *
 *	@returns True if the count up for the given timer is set, false otherwsie.
 */
bool ctr_timer_get_count_up(ctr_timer timer);

/**	Sets the state of the timer's up count flag.
 *
 *	3DS timers allow for their own count value to be incremented when the
 *	previous timer overflows by enabling the count up flag. Seeing that the
 *	first timer has no preceeding timer, enabling this flag for it should be
 *	useless. Enabling it for the other timers will cause for their count to
 *	increment when the previous timer's count overflows.
 *
 *	@param[in] timer Timer to set for its count up flag state.
 *
 *	@post If enabled, the timer will now only increase its count when the
 *		previous timer overflows. If disabled, the current timer increments on
 *		its own depending on a rate depending on its prescaler value.
 */
void ctr_timer_set_count_up(ctr_timer timer, bool enable);

/**	@brief Represent the prescaler values for the 3DS timers.
 */
typedef enum
{
	CTR_TIMER_DIV1 = 0,
	CTR_TIMER_DIV64 = 1,
	CTR_TIMER_DIV256 = 2,
	CTR_TIMER_DIV1024 = 3,
	CTR_TIMER_DIV_UNKNOWN
} ctr_timer_prescaler;

/**	@brief Sets the prescaler value for the given timer.
 *
 *	@param[in] timer Timer to set its prescaler value.
 *	@param[in] setting Prescaler setting to set.
 *
 *	@post The timer has had its prescaler value changed to the value represented
 *		by the prescaler enum, affecting the frequency of the timer.
 */
void ctr_timer_set_prescaler(ctr_timer timer, ctr_timer_prescaler setting);

/**	@brief Returns the current prescaler setting for the given timer.
 *
 *	@param[in] timer Timer to query for its prescaler setting.
 *
 *	@returns The current prescaler setting.
 */
ctr_timer_prescaler ctr_timer_get_prescaler(ctr_timer timer);

/**	@brief Returns the current frequency of the given timer in Hertz, rounded
 *		down to the nearest Hertz.
 *
 *	@param[in] timer Timer to query for the current effective frequency.
 *
 *	@returns The effective frequency of the given timer in Hz, rounded down.
 */
uint32_t ctr_timer_get_effective_frequency(ctr_timer timer);

#ifdef __cplusplus
}
#endif

#endif//CTR_TIMER_H_

