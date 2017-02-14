// Copyright 2014 Normmatt
// Licensed under GPLv2 or any later version
// Refer to the license.txt file included.

#include <ctr9/gamecart/command_ctr.h>
#include <ctr9/gamecart/protocol_ctr.h>

#include <stdint.h>
#include <stddef.h>

static int read_count = 0;

static void CTR_CmdC5(void)
{
	static const uint32_t c5_cmd[4] = { 0xC5000000, 0x00000000, 0x00000000, 0x00000000 };
	CTR_SendCommand(c5_cmd, 0, 1, 0x100002C, NULL);
}

void CTR_CmdReadData(uint32_t sector, uint32_t length, uint32_t blocks, void* buffer)
{
	if(read_count++ > 10000)
	{
		CTR_CmdC5();
		read_count = 0;
	}

	const uint32_t read_cmd[4] = {
		(0xBF000000 | (uint32_t)(sector >> 23)),
		(uint32_t)((sector << 9) & 0xFFFFFFFF),
		0x00000000, 0x00000000
	};
	CTR_SendCommand(read_cmd, length, blocks, 0x704822C, buffer);
}

void CTR_CmdReadHeader(void* buffer)
{
	static const uint32_t readheader_cmd[4] = { 0x82000000, 0x00000000, 0x00000000, 0x00000000 };
	CTR_SendCommand(readheader_cmd, 0x200, 1, 0x704802C, buffer);
}

uint32_t CTR_CmdGetSecureId(uint32_t rand1, uint32_t rand2)
{
	uint32_t id = 0;
	const uint32_t getid_cmd[4] = { 0xA2000000, 0x00000000, rand1, rand2 };
	CTR_SendCommand(getid_cmd, 0x4, 1, 0x701002C, &id);
	return id;
}

void CTR_CmdSeed(uint32_t rand1, uint32_t rand2)
{
	const uint32_t seed_cmd[4] = { 0x83000000, 0x00000000, rand1, rand2 };
	CTR_SendCommand(seed_cmd, 0, 1, 0x700822C, NULL);
}

