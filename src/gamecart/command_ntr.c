// Copyright 2014 Normmatt
// Licensed under GPLv2 or any later version
// Refer to the license.txt file included.

#include <ctr9/gamecart/command_ntr.h>
#include <ctr9/gamecart/protocol_ntr.h>

#include <stdint.h>
#include <stddef.h>

void NTR_CmdReset(void)
{
    static const uint32_t reset_cmd[2] = { 0x9F000000, 0x00000000 };
    NTR_SendCommand(reset_cmd, 0x2000, NTRCARD_CLK_SLOW | NTRCARD_DELAY1(0x1FFF) | NTRCARD_DELAY2(0x18), NULL);
}

uint32_t NTR_CmdGetCartId(void)
{
    uint32_t id;
    static const uint32_t getid_cmd[2] = { 0x90000000, 0x00000000 };
    NTR_SendCommand(getid_cmd, 0x4, NTRCARD_CLK_SLOW | NTRCARD_DELAY1(0x1FFF) | NTRCARD_DELAY2(0x18), &id);
    return id;
}

void NTR_CmdEnter16ByteMode(void)
{
    static const uint32_t enter16bytemode_cmd[2] = { 0x3E000000, 0x00000000 };
    NTR_SendCommand(enter16bytemode_cmd, 0x0, 0, NULL);
}
