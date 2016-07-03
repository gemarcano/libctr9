#include <stdint.h>
#include <stdbool.h>

#include <ctr9/ctr_pxi.h>

#define PXI_SYNC9 (*(uint32_t*)0x10008000)
#define PXI_CNT9 (*(uint32_t*)0x10008004)
#define PXI_SEND9 (*(uint32_t*)0x10008008)
#define PXI_RECV9 (*(uint32_t*)0x1000800C)

bool ctr_pxi_send_empty_status(void)
{
	return PXI_CNT9 & 0x1;
}

bool ctr_pxi_send_full_status(void)
{
	return PXI_CNT9 & (1 << 1);
}

bool ctr_pxi_get_send_empty_irq(void)
{
	return PXI_CNT9 & (1 << 2);
}

void ctr_pxi_set_send_empty_irq(bool aState)
{
	uint32_t cnt = PXI_CNT9;
	cnt &= ~(1u << 2);
	PXI_CNT9 = cnt | ((uint32_t)aState) << 2;
}

void ctr_pxi_fifo_send_clear(void)
{
	PXI_CNT9 |= 1 << 3;
}

bool ctr_pxi_receive_empty_status(void)
{
	return PXI_CNT9 & (1 << 8);
}

bool ctr_pxi_receive_full_status(void)
{
	return PXI_CNT9 & (1 << 9);
}

bool ctr_pxi_get_receive_not_empty_irq(void)
{
	return PXI_CNT9 & (1 << 10);
}

void ctr_pxi_set_receive_not_empty_irq(bool aState)
{
	uint32_t cnt = PXI_CNT9;
	cnt &= ~(1u << 10);
	PXI_CNT9 = cnt | ((uint32_t)aState) << 10;
}

void ctr_pxi_set_enabled(bool aState)
{
	uint32_t cnt = PXI_CNT9;
	cnt &= ~(1u << 15);
	PXI_CNT9 = cnt | ((uint32_t)aState) << 15;
}

bool ctr_pxi_get_enabled(void)
{
	return PXI_CNT9 & (1 << 15);
}

void ctr_pxi_fifo_ack(void)
{
	PXI_CNT9 |= 1 << 14;
}

bool ctr_pxi_get_error(void)
{
	return PXI_CNT9 & (1 << 14);
}

bool ctr_pxi_push(uint32_t aData)
{
	if (!ctr_pxi_send_full_status())
	{
		PXI_SEND9 = aData;
		return true;
	}
	return false;
}

bool ctr_pxi_pop(uint32_t *apData)
{
	if (!ctr_pxi_receive_empty_status())
	{
		*apData = PXI_RECV9;
		return true;
	}
	return false;
}

