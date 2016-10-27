/*******************************************************************************
 * Copyright (C) 2016 Gabriel Marcano
 *
 * Refer to the COPYING.txt file at the top of the project directory. If that is
 * missing, this file is licensed under the GPL version 2.0 or later.
 *
 ******************************************************************************/

/** @file */

#ifndef CTR_FATFS_DOTAB_H_
#define CTR_FATFS_DOTAB_H_

#ifdef __cplusplus
extern "C" {
#endif

int ctr_fatfs_dotab_initialize(void);

int ctr_fatfs_dotab_check_ready(const char *drive);

int ctr_fatfs_dotab_chdrive(const char *drive);

#ifdef __cplusplus
}
#endif


#endif//CTR_FATFS_DOTAB_H_

