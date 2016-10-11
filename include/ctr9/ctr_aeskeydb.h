#ifndef CTR_AESKEYDB_H_
#define CTR_AESKEYDB_H_

#include <stdint.h>

typedef struct
{
	uint8_t slot;
	char type;
	char id[10];
	uint8_t reserved[2];
	uint8_t is_devkit_key;
	uint8_t is_encrypted;
	uint8_t key[16];
} ctr_aesdb_entry;

void ctr_aesdb_entry_load(ctr_aesdb_entry *entry, const void *data);
void ctr_aesdb_entry_crypt_key(ctr_aesdb_entry *entry);

#endif//CTR_AESKEYDB_H_

