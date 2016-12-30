#include <ctr9/ctr_firm.h>

void ctr_firm_section_header_load(ctr_firm_section_header *header, const void *data)
{
	ctr_core_firm_section_header_load(header, data);
}

void ctr_firm_header_load(ctr_firm_header *header, const void *data)
{
	ctr_core_firm_header_load(header, data);
}

void ctr_arm9bin_header_load(ctr_arm9bin_header *header, const void *data)
{
	ctr_core_arm9bin_header_load(header, data);
}

