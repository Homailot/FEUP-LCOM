#ifndef _G01_UART_H_
#define _G01_UART_H_

#include <stdint.h>
#include <stdbool.h>

#include "networking.h"

/**
 * @brief Gets the line status from the LSR register of the UART.
 * @param[out] status The line status byte.
 * @return OK if no errors occurred.
 */
int (uart_get_line_status)(uint8_t * status);

/**
 * @brief Gets the line control from the LCR register of the UART.
 * @param[out] ctrl The line control byte.
 * @return OK if no errors occurred.
 */
int (uart_get_line_ctrl)(uint8_t * ctrl);

int (uart_get_int_id)(uint8_t * byte);

/**
 * @brief Writes a control byte to the LCR register of the UART.
 * @param ctrl The control byte.
 * @return OK if no errors occurred.
 */
int (uart_set_line_ctrl)(uint8_t ctrl);

/**
 * @brief Sends a byte through the UART.
 * @param byte The byte to be sent.
 * @return OK if no errors occurred.
 */
int (uart_send_byte)(uint8_t byte);

/**
 * @brief Sends a data packet through the UART.
 * @param pkt The packet to be sent
 * @return OK if no errors occurred.
 */
int (uart_send_data)(network_packet_t pkt);

/**
 * @brief Enables UART's fifo and clears both fifo's
 */
int (uart_init_fifo)();

/**
 * @brief Sets the baud rate (rate of symbols) of the UART.
 * @param rate The new rate.
 * @return OK if no errors occurred.
 */
int (uart_set_baud_rate)(uint16_t rate);

/**
 * @brief Checks and receives a byte from the UART.
 * @param[out] byte The received byte.
 * @return OK if no errors occurred.
 */
int(uart_receive_byte)(uint8_t * byte);

/**
 * @brief Subscribes to UART interrupts.
 * @param[out] bit_no The bit number used.
 * @return OK if no errors occurred.
 */
int (uart_subscribe_int)(uint8_t *bit_no);

/**
 * @brief Disables UART interrupts (given that they are subscribed to).
 * @return OK if no errors occurred.
 */
int(uart_disable_int)();

/**
 * @brief Enables UART interrupts (given that they are subscribed to).
 * @return OK if no errors occurred.
 */
int(uart_enable_int)();

/**
 * @brief Unsubscribes from UART interrupts.
 * @return OK if no errors occurred.
 */
int (uart_unsubscribe_int)();

/**
 * @brief Uart interrupt handler
 */
void (uart_ih)();

/**
 * @brief Initializes the queues related to the uart
 */
int (init_uart_queues)();

/**
 * @brief Frees the memory allocated by init_uart_queues
 */
int (free_uart_queues)();

/**
 * @brief Receives the front of the in_fifo.
 * @return true if not empty.
 */
bool (uart_recv_front)(uint8_t * byte);

/**
 *@brief Adds a byte to the transmit queue.
 */
void (uart_push_transmit)(uint8_t byte);

/**
 * @brief Checks if the transmitter empty interrupt was received, but nothing was sent.
 * @return True if the interrupt to transmit was called, but nothing was sent.
 */
bool (transmitter_empty)();

/**
 * @brief Sets the IER register of the UART
 * @return OK if no errors occurred.
 */ 
int (uart_set_interrupt_reg)(uint8_t ctrl);

void(empty_transmit_queue)();

#endif
