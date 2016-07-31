/*******************************************************************************
 * Copyright (C) 2016 Gabriel Marcano
 *
 * Refer to the COPYING.txt file at the top of the project directory. If that is
 * missing, this file is licensed under the GPL version 2.0 or later.
 *
 ******************************************************************************/

/** @file */

#ifndef CTR_SYSTEM_H_
#define CTR_SYSTEM_H_

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Enumeration describing the running system.
 */
typedef enum
{
    SYSTEM_O3DS,
    SYSTEM_N3DS,
} ctr_system_type;

/**	@brief Returns whether the arm9 entry was a9lh or not.
 *
 *	@returns True if the arm9 entrypoint was a9lh, false otherwise.
 */
bool ctr_detect_a9lh_entry(void);

/**	@brief Sets up the TWL keyslot.
 *
 *	This only really matters if arm9 execution is obtained via a9lh, or prior to
 *	a FIRM load. Nothing happens if FIRM had been launched previously.
 *
 *	@post The TWL keyslot is setup properly.
 */
void ctr_twl_keyslot_setup(void);

/**	@brief Powers off the 3DS.
 *
 *	This function does not return. It powers off the 3DS via an i2c call to the
 *	MPU.
 *
 *	@post 3DS has powered off.
 */
void ctr_system_poweroff(void);

/**	@brief Resets the 3DS.
 *
 *	This function does not return. It resets the 3DS via an i2c call to the MPU.
 *
 *	@post 3DS has restarted.
 */
void ctr_system_reset(void);

/**	@brief Returns the enumeration corresponding to the running system.
 *
 *	@returns The type of system this is called in.
 */
ctr_system_type ctr_get_system_type(void);

#ifdef __cplusplus
}
#endif

#endif//CTR_SYSTEM_H_

