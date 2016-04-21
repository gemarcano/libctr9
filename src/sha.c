#include <ctr9/sha.h>
#include <string.h>

void sha_init(uint32_t mode)
{
    while(*REG_SHACNT & 1);
    *REG_SHACNT = mode | SHA_CNT_OUTPUT_ENDIAN | SHA_NORMAL_ROUND;
}

void sha_update(const void* src, uint32_t size)
{    
    const uint32_t* src32 = (const uint32_t*)src;
    
    while(size >= 0x40) {
        while(*REG_SHACNT & 1);
        for(uint32_t i = 0; i < 4; i++) {
            *REG_SHAINFIFO = *src32++;
            *REG_SHAINFIFO = *src32++;
            *REG_SHAINFIFO = *src32++;
            *REG_SHAINFIFO = *src32++;
        }
        size -= 0x40;
    }
    while(*REG_SHACNT & 1);

    for(size_t i = 0; i < size; ++i)
    {
        ((volatile char*)REG_SHAINFIFO)[i] = ((const char*)src)[i];
    }
}

void sha_get(void* res) {
    *REG_SHACNT = (*REG_SHACNT & ~SHA_NORMAL_ROUND) | SHA_FINAL_ROUND;
    while(*REG_SHACNT & SHA_FINAL_ROUND);
    while(*REG_SHACNT & 1);

    for (size_t i = 0; i < 256u/8u; ++i)
    {
        ((char*)res)[i] = ((volatile char*)REG_SHAHASH)[i];
    }
}
