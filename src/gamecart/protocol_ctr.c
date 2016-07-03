// Copyright 2014 Normmatt
// Licensed under GPLv2 or any later version
// Refer to the license.txt file included.

#include <ctr9/gamecart/protocol_ctr.h>
#include <ctr9/gamecart/protocol.h>

#include <stddef.h>
#include <stdbool.h>

void ioDelay(uint32_t thing);

void CTR_SetSecKey(uint32_t value) {
    REG_CTRCARDSECCNT |= ((value & 3) << 8) | 4;
    while (!(REG_CTRCARDSECCNT & 0x4000));
}

void CTR_SetSecSeed(const uint32_t* seed, bool flag) {
    REG_CTRCARDSECSEED = BSWAP32(seed[3]);
    REG_CTRCARDSECSEED = BSWAP32(seed[2]);
    REG_CTRCARDSECSEED = BSWAP32(seed[1]);
    REG_CTRCARDSECSEED = BSWAP32(seed[0]);
    REG_CTRCARDSECCNT |= 0x8000;

    while (!(REG_CTRCARDSECCNT & 0x4000));

    if (flag) {
        (*(volatile uint32_t*)0x1000400C) = 0x00000001; // Enable cart command encryption?
    }
}

void CTR_SendCommand(const uint32_t command[4], uint32_t pageSize, uint32_t blocks, uint32_t latency, void* buffer)
{
#ifdef VERBOSE_COMMANDS
    Debug("C> %08X %08X %08X %08X", command[0], command[1], command[2], command[3]);
#endif

    REG_CTRCARDCMD[0] = command[3];
    REG_CTRCARDCMD[1] = command[2];
    REG_CTRCARDCMD[2] = command[1];
    REG_CTRCARDCMD[3] = command[0];

    //Make sure this never happens
    if(blocks == 0) blocks = 1;

    pageSize -= pageSize & 3; // align to 4 byte
    uint32_t pageParam = CTRCARD_PAGESIZE_4K;
    uint32_t transferLength = 4096;
    // make zero read and 4 byte read a little special for timing optimization(and 512 too)
    switch(pageSize) {
        case 0:
            transferLength = 0;
            pageParam = CTRCARD_PAGESIZE_0;
            break;
        case 4:
            transferLength = 4;
            pageParam = CTRCARD_PAGESIZE_4;
            break;
        case 64:
            transferLength = 64;
            pageParam = CTRCARD_PAGESIZE_64;
            break;
        case 512:
            transferLength = 512;
            pageParam = CTRCARD_PAGESIZE_512;
            break;
        case 1024:
            transferLength = 1024;
            pageParam = CTRCARD_PAGESIZE_1K;
            break;
        case 2048:
            transferLength = 2048;
            pageParam = CTRCARD_PAGESIZE_2K;
            break;
        case 4096:
            transferLength = 4096;
            pageParam = CTRCARD_PAGESIZE_4K;
            break;
	default:
	    break; //Defaults already set
    }

    REG_CTRCARDBLKCNT = blocks - 1;
    transferLength *= blocks;

    // go
    REG_CTRCARDCNT = 0x10000000;
    REG_CTRCARDCNT = /*CTRKEY_PARAM | */CTRCARD_ACTIVATE | CTRCARD_nRESET | pageParam | latency;

    uint8_t * pbuf = (uint8_t *)buffer;
    uint32_t * pbuf32 = (uint32_t * )buffer;
    bool useBuf = ( NULL != pbuf );
    bool useBuf32 = (useBuf && (0 == (3 & ((uint32_t)buffer))));

    uint32_t count = 0;
    uint32_t cardCtrl = REG_CTRCARDCNT;

    if(useBuf32)
    {
        while( (cardCtrl & CTRCARD_BUSY) && count < transferLength)
        {
            cardCtrl = REG_CTRCARDCNT;
            if( cardCtrl & CTRCARD_DATA_READY  ) {
                uint32_t data = REG_CTRCARDFIFO;
                *pbuf32++ = data;
                count += 4;
            }
        }
    }
    else if(useBuf)
    {
        while( (cardCtrl & CTRCARD_BUSY) && count < transferLength)
        {
            cardCtrl = REG_CTRCARDCNT;
            if( cardCtrl & CTRCARD_DATA_READY  ) {
                uint32_t data = REG_CTRCARDFIFO;
                pbuf[0] = (unsigned char) (data >>  0);
                pbuf[1] = (unsigned char) (data >>  8);
                pbuf[2] = (unsigned char) (data >> 16);
                pbuf[3] = (unsigned char) (data >> 24);
                pbuf += sizeof (unsigned int);
                count += 4;
            }
        }
    }
    else
    {
        while( (cardCtrl & CTRCARD_BUSY) && count < transferLength)
        {
            cardCtrl = REG_CTRCARDCNT;
            if( cardCtrl & CTRCARD_DATA_READY  ) {
                uint32_t data = REG_CTRCARDFIFO;
                (void)data;
                count += 4;
            }
        }
    }

    // if read is not finished, ds will not pull ROM CS to high, we pull it high manually
    if( count != transferLength ) {
        // MUST wait for next data ready,
        // if ds pull ROM CS to high during 4 byte data transfer, something will mess up
        // so we have to wait next data ready
        do { cardCtrl = REG_CTRCARDCNT; } while(!(cardCtrl & CTRCARD_DATA_READY));
        // and this tiny delay is necessary
        ioDelay(33);
        // pull ROM CS high
        REG_CTRCARDCNT = 0x10000000;
        REG_CTRCARDCNT = CTRKEY_PARAM | CTRCARD_ACTIVATE | CTRCARD_nRESET;
    }
    // wait rom cs high
    do { cardCtrl = REG_CTRCARDCNT; } while( cardCtrl & CTRCARD_BUSY );
    //lastCmd[0] = command[0];lastCmd[1] = command[1];

#ifdef VERBOSE_COMMANDS
    if (!useBuf) {
        Debug("C< NULL");
    } else if (!useBuf32) {
        Debug("C< non32");
    } else {
        uint32_t* p = (uint32_t*)buffer;
        int transferWords = count / 4;
        for (int i = 0; i < transferWords && i < 4*4; i += 4) {
            switch (transferWords - i) {
            case 0:
                break;
            case 1:
                Debug("C< %08X", p[i+0]);
                break;
            case 2:
                Debug("C< %08X %08X", p[i+0], p[i+1]);
                break;
            case 3:
                Debug("C< %08X %08X %08X", p[i+0], p[i+1], p[i+2]);
                break;
            default:
                Debug("C< %08X %08X %08X %08X", p[i+0], p[i+1], p[i+2], p[i+3]);
                break;
            }
        }
    }
#endif
}
