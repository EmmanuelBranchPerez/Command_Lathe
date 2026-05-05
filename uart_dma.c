 /*
 * uart_dma.c
 *
 *  Created on: Apr 14, 2026
 *      Author: emmanuel
 */

#include "uart_dma.h"
#include "string.h"

uint8_t rx_buffer_a[UART_RX_BUFFER_SIZE];
uint8_t rx_buffer_b[UART_RX_BUFFER_SIZE];

volatile UartPacket ready_packet;

static uint8_t active_buffer = 0;

void uart_dma_init(UART_HandleTypeDef *huart)
{
    // active_buffer = 0 then we should start filling buffer a
    HAL_UARTEx_ReceiveToIdle_DMA(huart, rx_buffer_a, UART_RX_BUFFER_SIZE);
}

int uart_dma_get_packet(UartPacket *out)
{
    // "Flag" checking if there's something in the buffer
    if (ready_packet.size == 0)
    {
        return 0;  // nothing
    }

    // We copy the information to a UartPacket struct
    out->buffer = ready_packet.buffer;
    out->size   = ready_packet.size;

    // Clear
    ready_packet.size = 0;
    ready_packet.buffer = 0;

    return 1; //ready
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
	if (huart->Instance == USART2)
	{
		if (active_buffer == 0)   // DMA was filling buffer A
		{
			HAL_UARTEx_ReceiveToIdle_DMA(huart, rx_buffer_b, UART_RX_BUFFER_SIZE);
			// Job 2: point ready_packet at buffer A
			ready_packet.buffer = rx_buffer_a;
			ready_packet.size = Size;
			// Job 3: raise the flag
			active_buffer = 1;
		}
		else                      // DMA was filling buffer B
		{
			HAL_UARTEx_ReceiveToIdle_DMA(huart, rx_buffer_a, UART_RX_BUFFER_SIZE);
			// Job 2: point ready_packet at buffer B
			ready_packet.buffer = rx_buffer_b;
			ready_packet.size = Size;
			// Job 3: raise the flag
			active_buffer = 0;
		}
	}
}
