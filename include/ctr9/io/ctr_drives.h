/*******************************************************************************
 * Copyright (C) 2016 Gabriel Marcano
 *
 * Refer to the COPYING.txt file at the top of the project directory. If that is
 * missing, this file is licensed under the GPL version 2.0 or later.
 *
 ******************************************************************************/

/** @file */

#ifndef CTR_DRIVES_H_
#define CTR_DRIVES_H_

#ifdef __cplusplus
extern "C" {
#endif

/**	@brief Initializes the standard library IO FILE subsystem.
 *
 *	@returns 0 on success, anything else on an error.
 */
int ctr_drives_initialize(void);

/**	@brief Checks whether the given drive is ready.
 *
 *	Possible drives are:
 *		"CTRNAND:"
 *		"SD:"
 *		"TWLN:"
 *		"TWLP:"
 *		"DISK0:"
 *		"DISK1:"
 *		"DISK2:"
 *		"DISK3:"
 *		"DISK4:"
 *		"DISK5:"
 *
 *	@param[in] Drive path to check if it is ready or not.
 *
 *	@returns 0 on success, anything else for an error indicating the drive is
 *		not ready for access via FILE functions.
 */
int ctr_drives_check_ready(const char *drive);

/**	@brief Changes the default drive used when no drive is specified in the
 *		path.
 *
 *	@param[in] drive Drive path to set as the default. See
 *		ctr_drives_check_ready for a list of drives supported.
 *
 *	@returns 0 on success, anything else on an error.
 */
int ctr_drives_chdrive(const char *drive);

#ifdef __cplusplus
}
#endif

#endif//CTR_DRIVES_H_

