#include <ctr9/ctr_system.h>
#include <ctr9/aes.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdalign.h>


#define PDN_MPCORE_CFG ((volatile uint8_t*)0x10140FFC)
#define PDN_SPI_CNT ((volatile uint8_t*)0x101401C0)

ctr_system_type ctr_get_system_type(void)
{
	//This is seemingly not confirmed on 3dbrew, but it seems PDN_MPCORE_CFG's
	//second and third bit are only set on the N3DS, while the first bit is
	//set for all systems. Use that to detect the type of system.
	return 0x07 == *PDN_MPCORE_CFG ? SYSTEM_N3DS : SYSTEM_O3DS;
}

bool ctr_detect_a9lh_entry(void)
{
	//Aurora determined that this register isn't yet set when a9lh launches.
	return *PDN_SPI_CNT == 0; 
}

void ctr_twl_keyslot_setup(void)
{
	//Only a9lh really needs to bother with this, and it really only needs to happen once, before ITCM gets messed up.
	static bool setup = false;
	if (!setup && ctr_detect_a9lh_entry())
	{
		uint32_t* TwlCustId = (uint32_t*) (0x01FFB808);
		uint32_t TwlKeyX[4];
		alignas(32) uint8_t TwlKeyY[16];

		// thanks b1l1s & Normmatt
		// see source from https://gbatemp.net/threads/release-twltool-dsi-downgrading-save-injection-etc-multitool.393488/
		const char* nintendo = "NINTENDO";
		TwlKeyX[0] = (TwlCustId[0] ^ 0xB358A6AF) | 0x80000000;
		TwlKeyX[3] = TwlCustId[1] ^ 0x08C267B7;
		memcpy(TwlKeyX + 1, nintendo, 8);

		// see: https://www.3dbrew.org/wiki/Memory_layout#ARM9_ITCM
		uint32_t TwlKeyYW3 = 0xE1A00005;
		memcpy(TwlKeyY, (uint8_t*) 0x01FFD3C8, 12);
		memcpy(TwlKeyY + 12, &TwlKeyYW3, 4);

		setup_aeskeyX(0x03, (uint8_t*)TwlKeyX);
		setup_aeskeyY(0x03, TwlKeyY);
		use_aeskey(0x03);
	}
	setup = true;
}

static void flushCache()
{
	asm volatile (
		"mov r0, pc \n\t"
		"bx r0 \n\t"

		".arm \n\t"
		"mov r1, #0 @segment \n\t"

		"outer_loop:"
			"mov r0, #0 @line \n\t"
			"inner_loop:"
				"orr r2, r1, r0 @make r2 be the resulting combination of segment and line \n\t"

				"mcr	p15, 0, r2, c7, c14, 2 @Clean and flush the line \n\t"

				"add r0, r0, #0x20 @move line to next \n\t"
				"cmp r0, #0x800 @(variable, #0x400 for inst or 0x800 for data) \n\t"
				"bne inner_loop \n\t"

			"add r1, r1, #0x40000000 \n\t"
			"cmp r1, #0x0 \n\t"
			"bne outer_loop \n\t"

		"mov r0, #0 \n\t"
		"mcr p15, 0, r0, c7, c5, 0 \n\t" //Flush instruction cache
		"mcr p15, 0, r0, c7, c10, 4 \n\t" //drain write buffer
		"orr r0, pc, #1 \n\t"
		"bx r0\n\t"
		".thumb\n\t"
		:::"r0","r1","r2","cc"
	);

}

