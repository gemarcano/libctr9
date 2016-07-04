// Copyright 2014 Normmatt
// Licensed under GPLv2 or any later version
// Refer to the license.txt file included.

#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void NTR_CmdReset(void);
uint32_t NTR_CmdGetCartId(void);
void NTR_CmdEnter16ByteMode(void);

#ifdef __cplusplus
}
#endif

