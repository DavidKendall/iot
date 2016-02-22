#ifndef __UART_POLLED_H
#define __UART_POLLED_H

#include <stdint.h>
#include <stdbool.h>

void uart0Init(uint32_t baudRate);
void uart0Put(uint8_t byte);
uint8_t uart0Get(void);
bool uart0ByteAvailable(void);
void uart2Init(uint32_t baudRate);
void uart2Put(uint8_t byte);
uint8_t uart2Get(void);
bool uart2ByteAvailable(void);
#endif