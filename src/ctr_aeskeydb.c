#include <ctr9/ctr_aeskeydb.h>
#include <ctr9/aes.h>
#include <string.h>
#include <stdalign.h>

#define LOAD_MEMBER(M,P) \
memcpy(&(M), P, sizeof(M)); \
P += sizeof(M)

void ctr_aesdb_entry_load(ctr_aesdb_entry *entry, const void *data)
{
	const uint8_t *entry_memory = (const uint8_t*)data;

	LOAD_MEMBER(entry->slot, entry_memory);
	LOAD_MEMBER(entry->type, entry_memory);
	LOAD_MEMBER(entry->id, entry_memory);
	LOAD_MEMBER(entry->reserved, entry_memory);
	LOAD_MEMBER(entry->is_devkit_key, entry_memory);
	LOAD_MEMBER(entry->is_encrypted, entry_memory);
	LOAD_MEMBER(entry->key, entry_memory);
}

void ctr_aesdb_entry_crypt_key(ctr_aesdb_entry *entry)
{
	use_aeskey(0x2c);
	alignas(4) uint8_t ctr[16] = {0};
	setup_aeskeyY(0x2c, ctr);
	ctr[0] = entry->slot;
	ctr[1] = entry->type;
	memcpy(ctr+2, entry->id, 10);
	set_ctr(ctr);
	ctr_decrypt(entry->key, entry->key, 1, AES_CNT_CTRNAND_MODE, ctr);
}

