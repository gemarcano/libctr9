// Copyright 2014 Normmatt
// Licensed under GPLv2 or any later version
// Refer to the license.txt file included.

#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void CTR_CmdReadSectorSD(uint8_t* aBuffer, uint32_t aSector);
void CTR_CmdReadData(uint32_t sector, uint32_t length, uint32_t blocks, void* buffer);
void CTR_CmdReadHeader(void* buffer);
uint32_t CTR_CmdGetSecureId(uint32_t rand1, uint32_t rand2);
void CTR_CmdSeed(uint32_t rand1, uint32_t rand2);

#ifdef __cplusplus
}
#endif

