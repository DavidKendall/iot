#include <stdint.h>
#include <stdbool.h>
#include <iolpc2378.h>
#include <board.h>
#include <bsp.h>
#include <uart_polled.h>

void uart0Init(uint32_t baudRate) {
  uint32_t div;

  /* Select TXD0 */
  PINSEL0 &= ~(1U << 5);
  PINSEL0 |= (1U << 4);

  /* Select RXD0 */
  PINSEL0 &= ~(1U << 7);
  PINSEL0 |= (1U << 6);

  U0LCR = 0x83; /* 8 bits, no Parity, 1 Stop bit, DLAB = 1 */
  div = (getFpclk(UART0_PCLK_OFFSET) / 16) / baudRate; /* baud rate */
  U0DLM = div / 256;
  U0DLL = div % 256;
  U0LCR = 0x03; /* DLAB = 0 */
  U0FCR = 0x07; /* Enable and reset FIFO */
  U0IER = 0;    /* no UART0 interrupts */
}

void uart0Put(uint8_t byte) {
  while (!(U0LSR & 0x20)) {
    /* skip */
  }
  U0THR = byte;
}

uint8_t uart0Get(void) {
  while (!uart0ByteAvailable()) {
    /* skip */
  }
  return U0RBR;
}
  
bool uart0ByteAvailable(void) {
  return ((U0LSR & 0x01) ? true : false);
}

void uart2Init(uint32_t baudrate) {
  uint32_t div;

  /* Enable power to UART2 */
  PCONP |= (1U << 24);

  /* Select TXD2 */
  PINSEL0 &= ~(1U << 21);
  PINSEL0 |= (1U << 20);

  /* Select RXD2 */
  PINSEL0 &= ~(1U << 23);
  PINSEL0 |= (1U << 22);

  U2LCR = 0x83; /* 8 bits, no Parity, 1 Stop bit */
  div = (getFpclk(UART2_PCLK_OFFSET) / 16) / baudrate; /*baud rate */
  U2DLM = div / 256;
  U2DLL = div % 256;
  U2LCR = 0x03; /* DLAB = 0 */
  U2FCR = 0x07; /* Enable and reset TX and RX FIFO. */
  U2IER = 0;  /* Disable UART2 interrupts */
}

void uart2Put(uint8_t byte) {
  while (!(U2LSR & 0x20)) {
    /* skip */
  }
  U2THR = byte;
}

uint8_t uart2Get(void) {
  while (!uart2ByteAvailable()) {
    /* skip */
  }
  return U2RBR;
}
  
bool uart2ByteAvailable(void) {
  return ((U2LSR & 0x01) ? true : false);
}


