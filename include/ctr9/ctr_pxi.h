/*******************************************************************************
 * Copyright (C) 2016 Gabriel Marcano
 *
 * Refer to the COPYING.txt file at the top of the project directory. If that is
 * missing, this file is licensed under the GPL version 2.0 or later.
 *
 ******************************************************************************/

/** @file */

#ifndef CTR_PXI_H_
#define CTR_PXI_H_

#include <ctr_core/ctr_core_pxi.h>

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

//PXI registers
#define PXI_SYNC11 (*(uint32_t*)0x10163000)
#define PXI_CNT11 (*(uint32_t*)0x10163004)
#define PXI_SEND11 (*(uint32_t*)0x10163008)
#define PXI_RECV11 (*(uint32_t*)0x1016300C)

/**	@brief Checks if the send PXI queue is empty.
 *
 *	@returns True if the send PXI queue is empty, false otherwise.
 */
bool ctr_pxi_send_empty_status(void);

/**	@brief Checks if the send PXI queue is full.
 *
 *	@returns True if the send PXI queue is full, false otherwise.
 */
bool ctr_pxi_send_full_status(void);

/**	@brief Checks the status of the IRQ for when the send PXI queue is empty.
 *
 *	@returns True if the IRQ is enabled, false otherwise.
 */
bool ctr_pxi_get_send_empty_irq(void);

/**	@brief Sets the state of the IRQ for when the send PXI queue is empty.
 *
 *	@param[in] state The state to set the IRQ to, true to enable, false to
 *		disable.
 *
 *	@post The IRQ will be enabled if thet parameter is true, or disabled if
 *		false.
 */
void ctr_pxi_set_send_empty_irq(bool state);

/**	@brief Clears/flushes the send PXI queue.
 *
 *	@post Clears/flushes the send PXI queue.
 */
void ctr_pxi_fifo_send_clear(void);

/**	@brief Checks if the receive PXI queue is empty.
 *
 *	@returns True if the receive PXI queue is empty, false otherwise.
 */
bool ctr_pxi_receive_empty_status(void);

/**	@brief Checks if the receive PXI queue is full.
 *
 *	@returns True if the receive PXI queue is full, false otherwise.
 */
bool ctr_pxi_receive_full_status(void);

/**	@brief Checks the status of the IRQ for when the receive PXI queue is not
 *		empty.
 *
 *	@returns True if the IRQ is enabled, false otherwise.
 */
bool ctr_pxi_get_receive_not_empty_irq(void);

/**	@brief Sets the state of the IRQ for when the receive PXI queue is not empty.
 *
 *	@param[in] state The state to set the IRQ to, true to enable, false to
 *		disable.
 *
 *	@post The IRQ will be enabled if thet parameter is true, or disabled if
 *		false.
 */
void ctr_pxi_set_receive_not_empty_irq(bool state);

/**	@brief Sets the state of the PXI system.
 *
 *	@param[in] state State to set the PXI system to.
 *
 * 	@post On setting the state to true, the PXI system is enabled. On false it
 * 		is disabled.
 */
void ctr_pxi_set_enabled(bool state);

/**	@brief Returns the state of the PXI system.
 *
 *	@brief Returns true if the PXI system is enabled, false otherwise.
 */
bool ctr_pxi_get_enabled(void);

/** @brief (FIXME Uncertain, this is most likely wrong) Acknowledge IRQ.
 */
void ctr_pxi_fifo_ack(void);

/**	@brief Returns the error status of the PXI system.
 *
 *	An error is set when a read is attempted from the read PXI queue and there
 *	is no data there, or when data is attempted to be sent when the send PXI
 *	queue is full. No errors are set when the system if offline.
 *
 *	@returns True if an error is reported by the PXI system, false otherwise.
 */
bool ctr_pxi_get_error(void);

/**	@brief Push data into the send PXI queue.
 *
 *	@param[in] data Data to push into send PXI queue.
 *
 *	@returns True if pushing succeeded, false if the queue was full and the push
 *		failed.
 */
bool ctr_pxi_push(uint32_t data);

/**	@brief Pops data from the receive PXI queue.
 *
 *	@param[in] data Pointer to where to store data popped from the receive PXI
 *		queue.
 *
 *	@returns True if popping succeeded, false if the queue was empty and the
 *		pop failed.
 */
bool ctr_pxi_pop(uint32_t *data);

#ifdef __cplusplus
}
#endif

#endif//CTR_PXI_H_

