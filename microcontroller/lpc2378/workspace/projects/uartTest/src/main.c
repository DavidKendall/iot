/*
 * An xbee transmission test 
 * via UART2
 *
 * DK - 08-02-2015
 */

#include <stdbool.h>
#include <stdint.h>
#include <bsp.h>
#include <lcd.h>
#include <buttons.h>
#include <delay.h>
#include <uart_polled.h>
#include <xbee.h>

enum {
  BUFLEN = 256
};

static uint8_t buffer[BUFLEN];

/*****************************************************************************
 **   Main Function  main()
 *****************************************************************************/
void main(void) {
  uint32_t packetLen;
  
  bspInit();
  uart2Init(9600);

  while (true) {
    packetLen = xbeeMkPacketFromString(buffer, "Hello");
    xbeeTxPacket(buffer, packetLen);
    // xbeeTxTestFrame(buffer);
    lcdSetTextPos(2,3);
    lcdWrite("%02x", buffer[packetLen - 1]);
    dly100us(10000);   
  }
}
/*****************************************************************************
 **                            End Of File
 *****************************************************************************/
