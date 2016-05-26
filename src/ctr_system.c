#include <ctr9/ctr_system.h>
#include <stdint.h>

#define PDN_MPCORE_CFG ((uint8_t*)0x10140FFC)

ctr_system_type ctr_get_system_type(void)
{
	//This is seemingly not confirmed on 3dbrew, but it seems PDN_MPCORE_CFG's
	//second and third bit are only set on the N3DS, while the first bit is
	//set for all systems. Use that to detect the type of system.
	return 0x07 == *PDN_MPCORE_CFG ? SYSTEM_N3DS : SYSTEM_O3DS;
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

