#ifndef __XBEE_H
#define __XBEE_H

uint32_t xbeeMkPacketFromString(uint8_t *packet, char *s);
void xbeeTxPacket(uint8_t *buffer, uint32_t packetLen);
void xbeeTxTestFrame(uint8_t *buffer);
uint8_t xbeeChecksum(uint8_t *dataBuffer, uint32_t pos);

#endif