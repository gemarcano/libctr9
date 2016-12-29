/*******************************************************************************
 * Copyright (C) 2016 Gabriel Marcano
 *
 * Refer to the COPYING.txt file at the top of the project directory. If that is
 * missing, this file is licensed under the GPL version 2.0 or later.
 *
 ******************************************************************************/

/** @file */

#ifndef CTR_ELF_LOADER_H_
#define CTR_ELF_LOADER_H_

#include <ctrelf.h>
#include <stdio.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

void ctr_load_header(Elf32_Ehdr *header, FILE *file);

int ctr_load_segment(const Elf32_Phdr *header, FILE *file);

int ctr_load_segments(const Elf32_Ehdr *header, FILE *file);

bool ctr_check_elf(Elf32_Ehdr *header);

#ifdef __cplusplus
}
#endif

#endif//CTR_ELF_LOADER_H_

