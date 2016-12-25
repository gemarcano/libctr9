#ifndef CTR_TEST_INTERRUPT_H_
#define CTR_TEST_INTERRUPT_H_

#include <stdint.h>

void abort_interrupt(uint32_t *registers, void *data);
void undefined_instruction(uint32_t *registers, void *data);
void prefetch_abort(uint32_t *registers, void *data);

#endif//CTR_TEST_INTERRUPT_H_

