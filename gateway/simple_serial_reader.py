from twisted.internet.serialport import SerialPort
from twisted.internet.protocol import Protocol
from twisted.internet import reactor
from json import dumps


GET_START         = 0
GET_LEN           = 1
GET_FRAME_TYPE    = 2
GET_REST_OF_FRAME = 3
START_BYTE        = b'\x7E'


class SNodeProtocol(Protocol):
  
  def __init__(self):
    self.state = GET_START
    self.buffer = bytearray()
    self.length = 0
    self.nodes = dict()
    self.badPacketCount = 0

  def cleanup(self):
    pass

  def connectionMade(self):
    print('Serial connection established')
    reactor.addSystemEventTrigger('before', 'shutdown', self.cleanup)

  def dataReceived(self, data):
    self.buffer += data     
    if self.state == GET_START:
      startPos = self.buffer.find(START_BYTE)
      if startPos >= 0:
        self.buffer = self.buffer[startPos:]
        print("Found start of frame : {:d}, {:d}".format(startPos, len(data)))
        self.state = GET_LEN
    elif self.state == GET_LEN:
      if len(self.buffer) >= 3:
        self.length = 256 * self.buffer[1] + self.buffer[2] + 3
        print('Length : {:d}'.format(self.length))
        if 0 <= self.length and self.length <= 72:
          self.state = GET_FRAME_TYPE
        else:
          self.buffer = bytearray()
          self.state = GET_START
    elif self.state == GET_FRAME_TYPE:
      self.frameType = self.buffer[3]
      print("Frame type: 0x{:2x}".format(self.frameType))
      self.state = GET_REST_OF_FRAME
    elif self.state == GET_REST_OF_FRAME:
      if len(self.buffer) >= self.length:
        packet = self.buffer[:self.length]
        print("Packet: "), 
        print(repr(packet))
        self.buffer = self.buffer[self.length:]
        print("Buffer: "),
        print(repr(self.buffer))
        if ((self.frameType == 0x90)  or (self.frameType == 0x91)):
          # startAppData = 32 if self.frameType == '\x90' else 38
          startAppData = packet.find("type:DATA")
          if startAppData > 0:
            print("Data starts at {:d}".format(startAppData))
            try:
              sensorData = dict([s.split(':') for s in str(packet[startAppData:self.length-1]).split(',')])
              sensorId = sensorData.get('id')
              if sensorId != None:
                msg = dumps(sensorData)
                print(msg)
                if sensorId not in self.nodes:
                  if self.frameType == 0x90:
                    self.nodes[sensorId] = {'longAddr' : packet[4:12], 'shortAddr' : packet[12:14]}
                  else:
                    self.nodes[sensorId] = {'longAddr' : packet[5:13], 'shortAddr' : packet[13:15]}
                  print("Nodes: " + repr(self.nodes))
            except ValueError:
              print("Dictionary error: " + repr(packet)) 
          else:
            print("Bad packet: " + repr(packet))
            self.badPacketCount += 1
        print("Bad Packet Count : {:d}\n".format(self.badPacketCount))
        self.state = GET_START
    else:
      print("UNKNOWN STATE")


if __name__ == '__main__':
   sp = SerialPort(SNodeProtocol(), '/dev/ttyUSB0', reactor, baudrate='115200')
   reactor.run()

