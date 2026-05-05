/*
 * uart_dma.h
 *
 *  Created on: Apr 16, 2026
 *      Author: emmanuel
 */

#ifndef INC_UART_DMA_H_
#define INC_UART_DMA_H_

#include "stm32f4xx_hal.h"

/*======================
  Buffer configuration
  ======================*/
#define UART_RX_BUFFER_SIZE  64   //Buffer size

// We use two buffers for ping pong method
extern uint8_t rx_buffer_a[UART_RX_BUFFER_SIZE];
extern uint8_t rx_buffer_b[UART_RX_BUFFER_SIZE];

// Struct for the transmission packet
typedef struct {
    uint8_t  *buffer;   // points to whichever station is ready
    uint16_t  size;      // how many valid bytes arrived
} UartPacket;

// flag
extern volatile UartPacket ready_packet;

/*======================
		API
  ======================*/
void uart_dma_init      (UART_HandleTypeDef *huart); // Initialize DMA
int  uart_dma_get_packet(UartPacket *out); // receive the packet
#endif /* INC_UART_DMA_H_ */
