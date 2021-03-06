#ifndef __XB_H
#define __XB_H

#include <mbed.h>

enum {
  XBEE_BUFFER_SIZE = 256
};

typedef uint8_t xbeeBuffer_t[XBEE_BUFFER_SIZE];

class Xb {
	public:
		Xb(PinName tx, PinName rx);
    uint32_t xbeeMkPacketFromString(xbeeBuffer_t packet, char *s);
	  void xbeeNodeDiscovery(void);
    void xbeeTxPacket(xbeeBuffer_t packet, uint32_t packetLen);
    bool xbeeReadable(void);
	  uint32_t xbeeReadRaw(xbeeBuffer_t buffer);
	  uint32_t xbeeReceivePacket(xbeeBuffer_t buffer);
   	uint32_t xbeeNetstring(xbeeBuffer_t buffer, char *str);
    void xbeeTxTestFrame(uint8_t *buffer);
    uint8_t xbeeChecksum(xbeeBuffer_t packet, uint32_t pos);	
	private:
		Serial serial;
};

#endif
