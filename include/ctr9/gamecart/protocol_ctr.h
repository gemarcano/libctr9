// Copyright 2014 Normmatt
// Licensed under GPLv2 or any later version
// Refer to the license.txt file included.

#pragma once

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define REG_CTRCARDCNT     (*(volatile uint32_t*)0x10004000)
#define REG_CTRCARDBLKCNT  (*(volatile uint32_t*)0x10004004)
#define REG_CTRCARDSECCNT  (*(volatile uint32_t*)0x10004008)
#define REG_CTRCARDSECSEED (*(volatile uint32_t*)0x10004010)
#define REG_CTRCARDCMD     ((volatile uint32_t*)0x10004020)
#define REG_CTRCARDFIFO    (*(volatile uint32_t*)0x10004030)

#define CTRCARD_PAGESIZE_0   (0<<16)
#define CTRCARD_PAGESIZE_4   (1u<<16)
#define CTRCARD_PAGESIZE_16  (2u<<16)
#define CTRCARD_PAGESIZE_64  (3u<<16)
#define CTRCARD_PAGESIZE_512 (4u<<16)
#define CTRCARD_PAGESIZE_1K  (5u<<16)
#define CTRCARD_PAGESIZE_2K  (6u<<16)
#define CTRCARD_PAGESIZE_4K  (7u<<16)
#define CTRCARD_PAGESIZE_16K (8u<<16)
#define CTRCARD_PAGESIZE_64K (9u<<16)

#define CTRCARD_CRC_ERROR    (1u<<4)
#define CTRCARD_ACTIVATE     (1u<<31)           // when writing, get the ball rolling
#define CTRCARD_IE           (1u<<30)           // Interrupt enable
#define CTRCARD_WR           (1u<<29)           // Card write enable
#define CTRCARD_nRESET       (1u<<28)           // value on the /reset pin (1 = high out, not a reset state, 0 = low out = in reset)
#define CTRCARD_BLK_SIZE(n)  (((n)&0xFu)<<16)   // Transfer block size

#define CTRCARD_BUSY         (1u<<31)           // when reading, still expecting incomming data?
#define CTRCARD_DATA_READY   (1u<<27)           // when reading, REG_CTRCARDFIFO has another word of data and is good to go

#define CTRKEY_PARAM 0x1000000u

void CTR_SetSecKey(uint32_t value);
void CTR_SetSecSeed(const uint32_t* seed, bool flag);

void CTR_SendCommand(const uint32_t command[4], uint32_t pageSize, uint32_t blocks, uint32_t latency, void* buffer);

#ifdef __cplusplus
}
#endif

