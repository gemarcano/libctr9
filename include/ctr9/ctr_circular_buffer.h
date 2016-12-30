#ifndef CTR_CIRCULAR_BUFFER_H_
#define CTR_CIRCULAR_BUFFER_H_

#include <ctr_core/ctr_core_circular_buffer.h>

#include <stddef.h>
#include <stdbool.h>

typedef ctr_core_circular_buffer ctr_circular_buffer;

void ctr_circular_buffer_initialize(ctr_circular_buffer *buffer, size_t size);
bool ctr_circular_buffer_push_back(ctr_circular_buffer *buffer, char data);
bool ctr_circular_buffer_pop_front(ctr_circular_buffer *buffer, char *data);
bool ctr_circular_buffer_get(ctr_circular_buffer *buffer, size_t index, char *data);
size_t ctr_circular_buffer_size(ctr_circular_buffer *buffer);
size_t ctr_circular_buffer_count(ctr_circular_buffer *buffer);

#endif//CTR_CIRCULAR_BUFFER_H_

