from twisted.internet.serialport import SerialPort
from twisted.internet.protocol import Protocol
from twisted.internet import reactor
from autobahn.twisted.websocket import WebSocketClientProtocol
from autobahn.twisted.websocket import WebSocketClientFactory
from json import dumps, loads
from mkxbeetx import xbeeMkPacketFromString, xbeeChecksum

class WSProtocol(WebSocketClientProtocol):

   def onConnect(self, response):
      print("Server connected: {0}".format(response.peer))

   def onOpen(self):
      print("WebSocket connection open.")
      self.sp = SerialPort(factory.sNodeProtocol(self), '/dev/ttyUSB0', reactor, baudrate='115200')

   def onMessage(self, payload, isBinary):
      if isBinary:
         print("Binary message received: {0}".format(int(payload)))
      else:
         data = loads(payload.decode('utf8'))
         if data[u'type'] == u'COMMAND':
           sensorId = data[u'id']
           print(repr(self.sp.protocol.nodes[sensorId]))
           longAddr = self.sp.protocol.nodes[sensorId]['longAddr']
           shortAddr = self.sp.protocol.nodes[sensorId]['shortAddr']
           if data[u'to'] == u'LED1_TOGGLE':
             packet = xbeeMkPacketFromString('1:1,', longAddr, shortAddr)
             print(["%02x" % ord(c) for c in packet])
             self.sp.protocol.transport.write(packet)
           elif data[u'to'] == u'LED2_TOGGLE':
             packet = xbeeMkPacketFromString('1:2,', longAddr, shortAddr)
             self.sp.protocol.transport.write(packet)
           elif data[u'to'] == u'LED3_TOGGLE':
             packet = xbeeMkPacketFromString('1:3,', longAddr, shortAddr)
             self.sp.protocol.transport.write(packet)
           elif data[u'to'] == u'LED4_TOGGLE':
             packet = xbeeMkPacketFromString('1:4,', longAddr, shortAddr)
             self.sp.protocol.transport.write(packet)
           elif data[u'to'] == u'SMOOTH_TOGGLE':
             packet = xbeeMkPacketFromString('1:5,', longAddr, shortAddr)
             self.sp.protocol.transport.write(packet)
           elif data[u'to'] == u'TEST_MESSAGE':
             packet = xbeeMkPacketFromString('1:6,', longAddr, shortAddr)
             self.sp.protocol.transport.write(packet)
           else:
             pass
           print(data)

   def onClose(self, wasClean, code, reason):
      print("WebSocket connection closed: {0}".format(reason))


GET_START         = 0
GET_LEN           = 1
GET_FRAME_TYPE    = 2
GET_REST_OF_FRAME = 3
START_BYTE        = b'\x7E'


class SNodeProtocol(Protocol):
  
  def __init__(self, websocket):
    self.state = GET_START
    self.buffer = bytearray()
    self.length = 0
    self.nodes = dict()
    self.ws = websocket

  def cleanup(self):
    print("\nAttempting to close websocket")
    self.ws.sendClose(3456, u"Cleaning up before reactor shutdown")

  def connectionMade(self):
    print('Serial connection established')
    reactor.addSystemEventTrigger('before', 'shutdown', self.cleanup)
    # self.sendLine('S')

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
        self.state = GET_FRAME_TYPE
    elif self.state == GET_FRAME_TYPE:
      self.frameType = self.buffer[3]
      print("%02x" % self.frameType)
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
          startAppData = 32 if self.frameType == '\x90' else 38
          print("Data starts at {:d}".format(packet.find("type:DATA")))
          try:
            sensorData = dict([s.split(':') for s in str(packet[startAppData:self.length-1]).split(',')])
            sensorId = sensorData.get('id')
            if sensorId != None:
              msg = dumps(sensorData)
              print(msg)
              self.ws.sendMessage(msg)
              if sensorId not in self.nodes:
                self.nodes[sensorId] = {'longAddr' : packet[4:12], 'shortAddr' : packet[12:14]}
                print(repr(self.nodes))
          except ValueError:
            print("Dictionary error: " + repr(packet)) 
        else:
          print("Bad packet: " + repr(packet))
        self.state = GET_START
    else:
      print("UNKNOWN STATE")


if __name__ == '__main__':
   factory = WebSocketClientFactory("ws://www.hesabu.net:9000/ws/sensors/rw", 
                                    debug = False)
   factory.protocol = WSProtocol
   factory.sNodeProtocol = SNodeProtocol
   reactor.connectTCP("www.hesabu.net", 9000, factory)
   reactor.run()
