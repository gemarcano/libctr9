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

int ctr_drives_initialize(void);

int ctr_drives_check_ready(const char *drive);

int ctr_drives_chdrive(const char *drive);

#ifdef __cplusplus
}
#endif

#endif//CTR_DRIVES_H_

