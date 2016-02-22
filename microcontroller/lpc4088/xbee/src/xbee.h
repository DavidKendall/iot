#ifndef __XBEE_H
#define __XBEE_H

#include <mbed.h>

enum {
  XBEE_BUFFER_SIZE = 256
};

typedef uint8_t xbeeBuffer_t[XBEE_BUFFER_SIZE];

class Xbee {
	public:
		Xbee(PinName tx, PinName rx);
    uint32_t xbeeMkPacketFromString(xbeeBuffer_t packet, char *s);
    void xbeeTxPacket(xbeeBuffer_t packet, uint32_t packetLen);
    uint32_t xbeeRxNetstring(xbeeBuffer_t buffer);
    void xbeeTxTestFrame(uint8_t *buffer);
    uint8_t xbeeChecksum(xbeeBuffer_t packet, uint32_t pos);	
	private:
		Serial serial;
};

#endif