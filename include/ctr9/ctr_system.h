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

#include <ctr_core/ctr_core_system.h>

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Enumeration describing the running system.
 */
typedef ctr_core_system_type ctr_system_type;

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
 *	@pre The areas of ITCM with TWL information are intact.
 *	@post TWL keys are setup. Note that this also does use the SHA subsystem, so
 *		the SHA register is modified. This means that under a9lh this overwrites
 *		the OTP hash, so back it up before calling this function.
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

/**	@brief Sets up the keyslot required for N3DS CTRNAND access.
 *
 *	@post Possible to use keyslot 0x5 to decrypt N3DS CTRNAND.
 */
void ctr_n3ds_ctrnand_keyslot_setup(void);

#ifdef __cplusplus
}
#endif

#endif//CTR_SYSTEM_H_

