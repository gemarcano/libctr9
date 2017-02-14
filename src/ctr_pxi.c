#include <stdint.h>
#include <stdbool.h>

#include <ctr9/ctr_pxi.h>

void ctr_pxi_initialize(void)
{
	ctr_core_pxi_change_base((volatile uint32_t*)0x10008000);
}

bool ctr_pxi_send_empty_status(void)
{
	return ctr_core_pxi_send_empty_status();
}

bool ctr_pxi_send_full_status(void)
{
	return ctr_core_pxi_send_full_status();
}

bool ctr_pxi_get_send_empty_irq(void)
{
	return ctr_core_pxi_get_send_empty_irq();
}

void ctr_pxi_set_send_empty_irq(bool aState)
{
	ctr_core_pxi_set_send_empty_irq(aState);
}

void ctr_pxi_fifo_send_clear(void)
{
	ctr_core_pxi_fifo_send_clear();
}

bool ctr_pxi_receive_empty_status(void)
{
	return ctr_core_pxi_receive_empty_status();
}

bool ctr_pxi_receive_full_status(void)
{
	return ctr_core_pxi_receive_full_status();
}

bool ctr_pxi_get_receive_not_empty_irq(void)
{
	return ctr_core_pxi_get_receive_not_empty_irq();
}

void ctr_pxi_set_receive_not_empty_irq(bool aState)
{
	ctr_core_pxi_set_receive_not_empty_irq(aState);
}

void ctr_pxi_set_enabled(bool aState)
{
	ctr_core_pxi_set_enabled(aState);
}

bool ctr_pxi_get_enabled(void)
{
	return ctr_core_pxi_get_enabled();
}

void ctr_pxi_fifo_ack(void)
{
	ctr_core_pxi_fifo_ack();
}

bool ctr_pxi_get_error(void)
{
	return ctr_core_pxi_get_error();
}

bool ctr_pxi_push(uint32_t aData)
{
	return ctr_core_pxi_push(aData);
}

bool ctr_pxi_pop(uint32_t *apData)
{
	return ctr_core_pxi_pop(apData);
}

