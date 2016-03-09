import struct

def xbeeChecksum(packet):
  sum = 0
  for i in range(3, len(packet) - 1):
    sum += packet[i]
  return 0xff - (sum & 0xff)


def xbeeMkPacketFromString(data, longAddr, shortAddr):
  packetLen = len(data) + 18
  packet = bytearray(packetLen)
  packet[0] = 0x7e
  packet[1:3] = struct.pack('>h', packetLen - 4)
  packet[3] = 0x10
  packet[4] = 0x00
  packet[5:13] = longAddr
  packet[13:15] = shortAddr
  packet[15] = 0x00
  packet[16] = 0x00
  packet[17:packetLen - 1] = data
  packet[packetLen - 1] = xbeeChecksum(packet)
  return str(packet)
