#ifndef CTR_PXI_H_
#define CTR_PXI_H_

#include <stdint.h>
#include <stdbool.h>

#define PXI_SYNC9 (*(uint32_t*)0x10008000)
#define PXI_CNT9 (*(uint32_t*)0x10008004)
#define PXI_SEND9 (*(uint32_t*)0x10008008)
#define PXI_RECV9 (*(uint32_t*)0x1000800C)

bool ctr_pxi_send_empty_status(void);

bool ctr_pxi_send_full_status(void);

bool ctr_pxi_get_send_empty_irq(void);

void ctr_pxi_set_send_empty_irq(bool aState);

void ctr_pxi_fifo_send_clear(void);

bool ctr_pxi_receive_empty_status(void);

bool ctr_pxi_receive_full_status(void);

bool ctr_pxi_get_receive_not_empty_irq(void);

void ctr_pxi_set_receive_not_empty_irq(bool aState);

void ctr_pxi_set_enabled(bool aState);

bool ctr_pxi_get_enabled(void);

void ctr_pxi_fifo_ack(void);

bool ctr_pxi_get_error(void);

bool ctr_pxi_push(uint32_t aData);

bool ctr_pxi_pop(uint32_t *apData);

#endif//CTR_PXI_H_

