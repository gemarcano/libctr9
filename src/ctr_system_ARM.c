#include <ctr9/ctr_system.h>

#ifdef __thumb__
#error This file cannot be compiled in as a Thumb target!
#endif

void ctr_flush_cache(void)
{
	/* The following is the C pseudo-code for what was implemented
	 * Refer to the ARM documentation for cleaning and flushing for
	 * the pseudo-code use to formulate this implementation, as well
	 * as for an explanation of the mcr opcode used.
	 
	uint32_t segment = 0;
	uint32_t line = 0;

	do
	{
		line = 0;
		do
		{
			uint32_t line_and_segment = line | segment;
			clean_and_flush(line_and_segment); //effectively the mcr opcode
	
			//The number of lines depends on the cache size, 0x800 for the
			//3DS data cache since it has 8KB of cache. Refer to the ARM
			//documentation for more details.
			line += 0x800;
		} while (line != 0x800);

		segment += 0x40000000;
	} while (segment != 0); //It overflows eventually

	flush_instruction_cache();
	drain_write_buffer();
	 
	 */

	asm volatile (
		"mov r1, #0 @segment \n\t"

		"1: \n\t" //outer_loop
			"mov r0, #0 @line \n\t"
			"2: \n\t" //inner_loop
				"orr r2, r1, r0 \n\t" //make r2 be the resulting combination of
				                      //segment and line

				"mcr p15, 0, r2, c7, c14, 2 \n\t" //Clean and flush the line 

				"add r0, r0, #0x20 \n\t" //move line to next line
				
				//The number of lines depends on the cache size, 0x800 for the
				//3DS data cache since it has 8KB of cache. Refer to the ARM
				//documentation for more details.
				"cmp r0, #0x800 \n\t"
				"bne 2b \n\t" //inner_loop

			"add r1, r1, #0x40000000 \n\t"
			"cmp r1, #0x0 \n\t"
			"bne 1b \n\t" //outer_loop

		"mov r0, #0 \n\t"
		"mcr p15, 0, r0, c7, c5, 0 \n\t" //Flush instruction cache
		"mcr p15, 0, r0, c7, c10, 4 \n\t" //drain write buffer
		:::"r0","r1","r2","cc"
	);
}

