#pragma once

#include <stdint.h>

#define REG_SHACNT      ((volatile uint32_t*)0x1000A000)
#define REG_SHABLKCNT   ((volatile uint32_t*)0x1000A004)
#define REG_SHAHASH     ((volatile uint32_t*)0x1000A040)
#define REG_SHAINFIFO   ((volatile uint32_t*)0x1000A080)

#define SHA_CNT_STATE           0x00000003u
#define SHA_CNT_OUTPUT_ENDIAN   0x00000008u
#define SHA_CNT_MODE            0x00000030u
#define SHA_CNT_ENABLE          0x00010000u
#define SHA_CNT_ACTIVE          0x00020000u

#define SHA_HASH_READY          0x00000000u
#define SHA_NORMAL_ROUND        0x00000001u
#define SHA_FINAL_ROUND         0x00000002u

#define SHA256_MODE             0
#define SHA224_MODE             0x00000010u
#define SHA1_MODE               0x00000020u


void sha_init(uint32_t mode);
void sha_update(const void* src, uint32_t size);
void sha_get(void* res);

