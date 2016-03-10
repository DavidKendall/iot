#include <mbed.h>
#include "xb.h"

Xb::Xb(PinName tx, PinName rx) :
  serial(tx, rx) {
	
  serial.baud(115200);
	serial.format(8, Serial::None, 1);
}
	
uint32_t Xb::xbeeMkPacketFromString(xbeeBuffer_t packet, char *s) {
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
  packet[4] = 0x00;
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

void Xb::xbeeNodeDiscovery() {
	xbeeBuffer_t packet;
	
	packet[0] = 0x7E;
	packet[1] = 0x00;
	packet[2] = 0x04;
	packet[3] = 0x08;
	packet[4] = 0x01;
	packet[5] = 0x4E;
	packet[6] = 0x44;
	packet[7] = 0x64;	
	
	xbeeTxPacket(packet, 8);
}

void Xb::xbeeTxPacket(xbeeBuffer_t packet, uint32_t packetLen) {
  uint32_t i;
  
  for (i = 0; i < packetLen; i++) {
    serial.putc(packet[i]);
  }
}

bool Xb::xbeeReadable(void) {
	return serial.readable() ? true : false;
}

uint32_t Xb::xbeeReadRaw(xbeeBuffer_t buffer) {
	uint32_t i = 0;
	
	while (serial.readable() && (i < XBEE_BUFFER_SIZE)) {
		buffer[i] = serial.getc();
		i += 1;
	}
	return i;
}

uint32_t Xb::xbeeReceivePacket(xbeeBuffer_t buffer) {
	uint32_t length = 0;
	uint32_t i;
	
	while (true) {
		buffer[0] = serial.getc();
		if (buffer[0] == 0x7E) {
			buffer[1] = serial.getc();
			buffer[2] = serial.getc();
			buffer[3] = serial.getc();
			if ((buffer[3] == 0x90) || (buffer[3] == 0x91)) {
			  length = buffer[1] * 256 + buffer[2];
			  if (length <= 84) {
			  	for (i = 4; i <= length + 1; i += 1) {
				  	buffer[i] = serial.getc();
				  }
					break;
			  }	
			}				
		}
	}
	return length + 4;
}

uint32_t Xb::xbeeNetstring(xbeeBuffer_t buffer, char *str) {
  uint8_t b;
  uint32_t length = 0;
  uint32_t i;
	uint32_t j;
  bool readingLength = true;
  
	i = 15;
  while (readingLength) {
		b = buffer[i];
    if (b == ':') {
      readingLength = false;
    } else {
      length = length * 10 + (b - 48);
    }
		i += 1;
  }
  for (j = 0; j < length; j += 1, i += 1) {
    str[j] = buffer[i];
  }
	return length;
}


uint8_t Xb::xbeeChecksum(xbeeBuffer_t packet, uint32_t pos) {
  uint32_t sum = 0;
  uint32_t i;
  
  for (i = 3; i < pos; i++) {
    sum += packet[i];
  }
  return ((uint8_t) (0xff - (sum & 0xff)));
}

void Xb::xbeeTxTestFrame(uint8_t *buffer) {
  static uint8_t helloBuffer[] = {0x7E, 0x00, 0x13, 0x10, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFE, 0x00, 0x00, 0x48, 0x65, 0x6C, 0x6C, 0x6F, 0xFD};
  uint32_t i;
  
  for (i = 0; i < 23; i++) {
    buffer[i] = helloBuffer[i];
  }
  xbeeTxPacket(buffer, 23);
}


