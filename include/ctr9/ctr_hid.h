/*******************************************************************************
 * Copyright (C) 2016 Gabriel Marcano
 *
 * Refer to the COPYING.txt file at the top of the project directory. If that is
 * missing, this file is licensed under the GPL version 2.0 or later.
 *
 ******************************************************************************/

/** @file */

#ifndef CTR_HID_H_
#define CTR_HID_H_

#include <ctr_core/ctr_core_hid.h>

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

//Button type
typedef ctr_core_hid_button_type ctr_hid_button_type;

//HID Register
#define CTR_HID_REG     CTR_CORE_HID_REG

//Button definitions
#define CTR_HID_NONE    CTR_CORE_HID_NONE
#define CTR_HID_A       CTR_CORE_HID_A
#define CTR_HID_B       CTR_CORE_HID_B
#define CTR_HID_SELECT  CTR_CORE_HID_SELECT
#define CTR_HID_START   CTR_CORE_HID_START
#define CTR_HID_RIGHT   CTR_CORE_HID_RIGHT
#define CTR_HID_LEFT    CTR_CORE_HID_LEFT
#define CTR_HID_UP      CTR_CORE_HID_UP
#define CTR_HID_DOWN    CTR_CORE_HID_DOWN
#define CTR_HID_RT      CTR_CORE_HID_RT
#define CTR_HID_LT      CTR_CORE_HID_LT
#define CTR_HID_X       CTR_CORE_HID_X
#define CTR_HID_Y       CTR_CORE_HID_Y

/**	@brief Returns whether the given button combination is pressed or not.
 *
 *	@param[in] buttons Button combination to check if they are pressed.
 *
 *	@returns True if the buttons passed in are pressed, false otherwise.
 */
bool ctr_hid_button_status(ctr_hid_button_type buttons);

/**	@brief Returns all of the buttons pressed at the instant this is called.
 *
 *	@brief returns All of the buttons pressed at the time this function is
 *		called.
 */
ctr_hid_button_type ctr_hid_get_buttons(void);

void ctr_input_wait(void);

#ifdef __cplusplus
}
#endif

#endif//CTR_HID_H_

