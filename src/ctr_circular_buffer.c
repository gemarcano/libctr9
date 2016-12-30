#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>

#include <ctr9/ctr_circular_buffer.h>

void ctr_circular_buffer_initialize(ctr_circular_buffer *buffer, size_t size)
{
	ctr_core_circular_buffer_initialize(buffer, size);
}

bool ctr_circular_buffer_push_back(ctr_circular_buffer *buffer, char data)
{
	return ctr_core_circular_buffer_push_back(buffer, data);
}

bool ctr_circular_buffer_pop_front(ctr_circular_buffer *buffer, char *data)
{
	return ctr_core_circular_buffer_pop_front(buffer, data);;
}

bool ctr_circular_buffer_get(ctr_circular_buffer *buffer, size_t index, char *data)
{
	return ctr_core_circular_buffer_get(buffer, index, data);
}

size_t ctr_circular_buffer_size(ctr_circular_buffer *buffer)
{
	return ctr_core_circular_buffer_size(buffer);
}

size_t ctr_circular_buffer_count(ctr_circular_buffer *buffer)
{
	return ctr_core_circular_buffer_count(buffer);
}

