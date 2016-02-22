#include <stdbool.h>
#include <stdint.h>
#include <uart_polled.h>
#include <xbee.h>

uint32_t xbeeMkPacketFromString(xbeeBuffer_t packet, char *s) {
  uint8_t payloadLen;
  uint8_t packetLen;
  uint32_t i;
  uint32_t j;

  i = 0;
  j = 17;
  while (s[i] != 0) {
    packet[j] = s[i];
    i++;
    j++;
  }
  payloadLen = i;
  packetLen = payloadLen + 18;
  packet[0] = 0x7e;
  packet[1] = 0x00;
  packet[2] = packetLen - 4;
  packet[3] = 0x10;
  packet[4] = 0x01;
  packet[5] = 0x00;
  packet[6] = 0x00;
  packet[7] = 0x00;
  packet[8] = 0x00;
  packet[9] = 0x00;
  packet[10] = 0x00;
  packet[11] = 0x00;
  packet[12] = 0x00;
  packet[13] = 0xff;
  packet[14] = 0xfe;
  packet[15] = 0x00;
  packet[16] = 0x00;
  packet[packetLen-1] = xbeeChecksum(packet, packetLen-1);
  for (i = packetLen; i < XBEE_BUFFER_SIZE; i += 1) {
    packet[i] = 0x00;
  }
  return packetLen;
}

void xbeeTxPacket(xbeeBuffer_t packet, uint32_t packetLen) {
  uint32_t i;
  
  for (i = 0; i < packetLen; i++) {
    uart2Put(packet[i]);
  }
}

uint32_t xbeeRxNetstring(xbeeBuffer_t buffer) {
  uint8_t b;
  uint32_t length = 0;
  uint32_t i;
  bool readingLength = true;
  
  while (readingLength) {
    b = uart2Get();
    if (b == ':') {
      readingLength = false;
    } else {
      length = length * 10 + (b - 48);
    }
  }
  if (length > XBEE_BUFFER_SIZE) {
    length = XBEE_BUFFER_SIZE;  // notice that we silently truncate data that is too big for the buffer
  }
  for (i = 0; i < length; i += 1) {
    buffer[i] = uart2Get();
  }
  b = uart2Get(); // read the trailing comma on the netstring
  return length;
}


uint8_t xbeeChecksum(xbeeBuffer_t packet, uint32_t pos) {
  uint32_t sum = 0;
  uint32_t i;
  
  for (i = 3; i < pos; i++) {
    sum += packet[i];
  }
  return ((uint8_t) (0xff - (sum & 0xff)));
}

void xbeeTxTestFrame(uint8_t *buffer) {
  static uint8_t helloBuffer[] = {0x7E, 0x00, 0x13, 0x10, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFE, 0x00, 0x00, 0x48, 0x65, 0x6C, 0x6C, 0x6F, 0xFD};
  uint32_t i;
  
  for (i = 0; i < 23; i++) {
    buffer[i] = helloBuffer[i];
  }
  xbeeTxPacket(buffer, 23);
}


