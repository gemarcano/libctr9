/*******************************************************************************
 * Copyright (C) 2016 Gabriel Marcano
 *
 * Refer to the COPYING.txt file at the top of the project directory. If that is
 * missing, this file is licensed under the GPL version 2.0 or later.
 *
 ******************************************************************************/

#include <ctr9/ctr_elf_loader.h>
#include <ctr9/ctr_cache.h>

#include <limits.h>
#include <string.h>

void ctr_load_header(Elf32_Ehdr *header, FILE *file)
{
	fseek(file, 0, SEEK_SET);
	char buffer[sizeof(*header)];
	fread(buffer, sizeof(buffer), 1, file);
	elf_load_header(header, buffer);
}

static int set_position(FILE *file, uint64_t position)
{
	if (fseek(file, 0, SEEK_SET)) return -1;
	while (position > LONG_MAX)
	{
		long pos = LONG_MAX;
		if (fseek(file, pos, SEEK_CUR)) return -1;
		position -= LONG_MAX;
	}

	if (fseek(file, position, SEEK_CUR)) return -1;
	return 0;
}

int ctr_load_segment(const Elf32_Phdr *header, FILE *file)
{
	size_t program_size = header->p_filesz;
	size_t mem_size = header->p_memsz;
	void *location = (void*)(header->p_vaddr);

	size_t type = header->p_type;

	switch (type)
	{
		case PT_LOAD:
			break;
		default:
			return 1;
	}

	set_position(file, header->p_offset);
	fread(location, program_size, 1, file);
	memset(program_size + (char*)location, 0, mem_size - program_size);

	ctr_cache_clean_data_range(location, (char*)location + mem_size);
	ctr_cache_flush_instruction_range(location, (char*)location + mem_size);
	ctr_cache_drain_write_buffer();

	return 0;
}

int ctr_load_segments(const Elf32_Ehdr *header, FILE *file)
{
	int res = 0;
	size_t pnum = header->e_phnum;
	char buffer[pnum][header->e_phentsize];

	set_position(file, header->e_phoff);
	res = 1 != fread(buffer, sizeof(buffer), 1, file);

	if (res)
		return res;

	for (size_t i = 0; i < pnum; ++i)
	{
		Elf32_Phdr pheader;
		elf_load_program_header(&pheader, buffer[i]);
		res = ctr_load_segment(&pheader, file);
		if (res)
			return res;
	}

	return res;
}

bool ctr_check_elf(Elf32_Ehdr *header)
{
	if (!(header->e_ident[EI_MAG0] == (char)0x7f &&
			header->e_ident[EI_MAG1] == 'E' &&
			header->e_ident[EI_MAG2] == 'L' &&
			header->e_ident[EI_MAG3] == 'F'))
		return false;

	if (header->e_ident[EI_CLASS] != 1)
		return false;

	if (header->e_ident[EI_DATA] != 1)
		return false;

	if (header->e_ident[EI_VERSION] != EV_CURRENT)
		return false;

	if (header->e_type != ET_EXEC)
		return false;

	if (header->e_machine != EM_ARM)
		return false;

	if (header->e_version != EV_CURRENT)
		return false;

	if (header->e_entry == 0)
		return false;

	if (header->e_phoff == 0)
		return false;

	if (header->e_ehsize == 0)
		return false;
	
	return true;
}

