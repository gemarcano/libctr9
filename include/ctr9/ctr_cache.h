/*******************************************************************************
 * Copyright (C) 2016, 2017 Gabriel Marcano
 *
 * Refer to the COPYING.txt file at the top of the project directory. If that is
 * missing, this file is licensed under the GPL version 2.0 or later.
 *
 ******************************************************************************/

/** @file */

#ifndef CTR_CACHE_H_
#define CTR_CACHE_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**	@brief Cleans and flushes all caches and write buffers.
 *
 *	@post All of the caches have been invalidated, and the data cache has been
 *		cleaned. The write buffer has been drained.
 */
void ctr_cache_clean_and_flush_all(void);

/**	@brief Flushes (invalidates) all of the data cache.
 *
 *	@post The entire data cache is invalidated.
 */
void ctr_cache_flush_data_all(void);

/**	@brief Flushes (invalidates) the entry in the cache associated with the
 *		given address, if any exists.
 *
 *	The entry is associated with an address is the one specified by taking the
 *	address and clearing the bottom 5 bits.
 *
 *	@param[in] address Address in memory to make sure to flush from cache.
 *
 *	@post The entry in the cache associated with the given address is
 *		invalidated.
 */
void ctr_cache_flush_data_entry(void *address);

/**	@brief Cleans but does not flush (invalidate) the entry in the cache
 *		associated with the given address, if any exists.
 *
 *	The entry is associated with an address is the one specified by taking the
 *	address and clearing the bottom 5 bits.
 *
 *	@param[in] address Address in memory to make sure to clean in cache.
 *
 *	@post The entry in the cache associated with the given address is cleaned.
 */
void ctr_cache_data_clean_entry(void *address);

/**	@brief Cleans and flushes the entry in the cache associated with the given
 *		address, if any exists.
 *
 *	The entry is associated with an address is the one specified by taking the
 *	address and clearing the bottom 5 bits.
 *
 *	@param[in] address Address in memory to make sure to clean and flush from
 *		cache.
 *
 *	@post The entry in the cache associated with the given address is cleaned
 *		and flushed.
 */
void ctr_cache_data_clean_and_flush_entry(void *address);

/**	@brief Cleans but does not flush (invalidate) the entry in the cache
 *		associated with the given segment and index.
 *
 *	@param[in] segment (FIXME what exactly is a segment? Other than there are 4
 *		of them?). Value must be less than 4.
 *	@param[in] index (FIXME what exactly is an index? Other than there are 64 of
 *		these). Value must be less than 64.
 *
 *	@post The cache entry specified by the segment and index is cleaned.
 */
void ctr_cache_data_clean_index_entry(uint8_t segment, uint8_t index);

/**	@brief Cleans and flushes the entry in the cache associated with the given
 *		segment and index.
 *
 *	@param[in] segment (FIXME what exactly is a segment? Other than there are 4
 *		of them?). Value must be less than 4.
 *	@param[in] index (FIXME what exactly is an index? Other than there are 64 of
 *		these). Value must be less than 64.
 *
 *	@post The cache entry specified by the segment and index is cleaned and
 *		flushed.
 */
void ctr_cache_data_clean_and_flush_index_entry(uint8_t segment, uint8_t index);

/**	@brief Flushes (invalidates) all instruction cache.
 *
 *	@post All of the instruction cache is invalidated.
 */
void ctr_cache_flush_instruction_all(void);

/**	@brief Flushes (invalidates) the instructions in the entry associated with
 *		the given address, if there is any.
 *
 *	The entry is associated with an address is the one specified by taking the
 *	address and clearing the bottom 5 bits.
 *
 *	@param[in] address Address in memory to make sure to flush from cache.
 *
 *	@post The cache entry associated with the given address is invalidated.
 */
void ctr_cache_flush_instruction_entry(void *address);

/**	@brief Pre-fetches the line that includes the given address. Cache lines are
 *		32 bytes long in the 3DS ARM9 instruction cache.
 *
 *	The entry is associated with an address is the one specified by taking the
 *	address and clearing the bottom 5 bits.
 *
 *	@param[in] address Address in line of memory to prefetch.
 */
void ctr_cache_prefetch_instruction_line(void *address);

/**	@brief Cleans and flushes the data cache for the range of memory specified.
 *
 *	@param[in] start Starting address in the range.
 *	@param[in] end Ending address, exclusive.
 *
 *	post The range specified by the given parameters is cleaned and flushed.
 */
void ctr_cache_clean_and_flush_data_range(void *start, void *end);

/**	@brief Cleans the data cache for the range of memory specified.
 *
 *	@param[in] start Starting address in the range.
 *	@param[in] end Ending address, exclusive.
 *
 *	@post The range specified by the given parameters is cleaned.
 */
void ctr_cache_clean_data_range(void *start, void *end);

/**	@brief Flushes (invalidates) the data cache for the range of memory
 *		specified.
 *
 *	@param[in] start Starting address in the range.
 *	@param[in] end Ending address, exclusive.
 *
 *	@post The range specified by the given parameters is invalidated.
 */
void ctr_cache_flush_data_range(void *start, void *end);

/**	@brief Flushes (invalidates) the instruction cache for the range of memory
 *		specified.
 *
 *	@param[in] start Starting address in the range.
 *	@param[in] end Ending address, exclusive.
 *
 *	@post The range specified by the given parameters is invalidated.
 */
void ctr_cache_flush_instruction_range(void *start, void *end);

/**	@brief Drains the write buffer.
 *
 *	@post The write buffer has been drained-- all data pending to be written to
 *		memory has been written.
 */
void ctr_cache_drain_write_buffer(void);

#ifdef __cplusplus
}
#endif

#endif//CTR_CACHE_H_

