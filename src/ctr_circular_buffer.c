#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>

#include <ctr9/ctr_circular_buffer.h>

void ctr_circular_buffer_initialize(ctr_circular_buffer *buffer, size_t size)
{
	buffer->size = size;
	buffer->buffer = malloc(size);
	buffer->count = 0;
	buffer->begin = buffer->buffer;
	buffer->end = buffer->buffer;
}


bool ctr_circular_buffer_push_back(ctr_circular_buffer *buffer, char data)
{
	if (buffer->count != buffer->size)
	{
		*buffer->end = data;
		buffer->count++;
	}
	else
		return false;

	if ((unsigned)(buffer->end - buffer->buffer) == buffer->size - 1)
	{
		buffer->end = buffer->buffer;
	}
	else
	{
		buffer->end++;
	}
	return true;
}

bool ctr_circular_buffer_pop_front(ctr_circular_buffer *buffer, char *data)
{
	if (buffer->count != 0)
	{
		if (data)
			*data = *buffer->begin;
		buffer->count--;
	}
	else
		return false;

	if ((unsigned)(buffer->begin - buffer->buffer) == buffer->size - 1)
	{
		buffer->begin = buffer->buffer;
	}
	else
	{
		buffer->begin++;
	}
	return true;
}

bool ctr_circular_buffer_get(ctr_circular_buffer *buffer, size_t index, char *data)
{
	if (index > buffer->count) return false;

	unsigned char *ptr = buffer->begin + index;
	if (ptr >= buffer->buffer + buffer->size)
	{
		ptr -= buffer->size;
	}
	*data = *ptr;
	return true;
}

size_t ctr_circular_buffer_size(ctr_circular_buffer *buffer)
{
	return buffer->size;
}

size_t ctr_circular_buffer_count(ctr_circular_buffer *buffer)
{
	return buffer->count;
}

