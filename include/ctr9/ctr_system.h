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

/** @brief Enumeration describing the running system.
 */
typedef enum
{
    SYSTEM_O3DS,
    SYSTEM_N3DS,
} ctr_system_type;

/**	@brief Returns the enumeration corresponding to the running system.
 *
 *	@returns The type of system this is called in.
 */
ctr_system_type ctr_get_system_type(void);

#endif//CTR_SYSTEM_H_

