// Copyright 2014 Normmatt
// Licensed under GPLv2 or any later version
// Refer to the license.txt file included.

#pragma once
#include <stdint.h>

#define REG_CARDCONF  (*(volatile uint16_t*)0x1000000C)
#define REG_CARDCONF2 (*(volatile uint8_t*)0x10000010)

//REG_AUXSPICNT
#define CARD_ENABLE     (1u<<15)
#define CARD_SPI_ENABLE (1u<<13)
#define CARD_SPI_BUSY   (1u<<7)
#define CARD_SPI_HOLD   (1u<<6)

#define LATENCY 0x822Cu

uint32_t BSWAP32(uint32_t val);

void Cart_Init(void);
int Cart_IsInserted(void);
uint32_t Cart_GetID(void);
void Cart_Secure_Init(uint32_t* buf, uint32_t* out);
void Cart_Dummy(void);
