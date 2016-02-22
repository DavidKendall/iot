import tornado.httpserver
import tornado.websocket
import tornado.ioloop
import tornado.web
import os
import logging
import datetime
subscribers = {}
publishers = {}


class WSHandler(tornado.websocket.WebSocketHandler):
    def check_origin(self, origin):
        return True

    def open(self, chan, mode):
        self.channel = chan
        self.mode = mode
        if mode == 'rw' or mode == 'ro':
            if not subscribers.get(self.channel):
                subscribers[self.channel] = []
            subscribers[self.channel].append(self)	
            logging.warning("New Subscribers: chan %s, mode: %s"%(chan, mode))
        elif mode == 'wo':
            publishers[self] = {'stream':self, 'lastact':datetime.datetime.now()}	
            logging.warning("New Publishers: chan %s, mode: %s"%(chan, mode))

    def on_message(self, message):
        for client in subscribers.get(self.channel, []):
	          client.write_message(message)

        if self.mode == 'wo':
            if self in publishers.keys():
                publishers[self]['lastact'] = datetime.datetime.now()
            else:
			          #client is not in publishers. Server must have restarted.
                publishers[self] = {'stream':self, 'lastact':datetime.datetime.now()}	
			
		

    def on_close(self):	
	logging.warning("Client closing. Chan: %s, Mode: %s"%(self.channel, self.mode))
        if self.mode == 'rw' or self.mode == 'ro':
            subscribers[self.channel].remove(self)
            if len(subscribers[self.channel]) == 0:
                del subscribers[self.channel]
        elif self.mode == 'wo':
                        self.close()
                        self.stream.close()
		        try:
		            del publishers[self]
		        except:
			          pass


settings = { 'auto_reload': True }

application = tornado.web.Application([
    (r'/ws/(.*)/(.*)', WSHandler),
], **settings)

def test():
	for k,v in publishers.items():
		delta = datetime.datetime.now() - v['lastact'] 
		if delta.seconds > 1:
			logging.warning("Callback. Client %s did not send data in the last 2 secs. Closing"%k)
		        del publishers[k]
			v['stream'].stream.close()
			
def noTest():
  pass

if __name__ == "__main__":
    http_server = tornado.httpserver.HTTPServer(application)
    http_server.listen(9000)
    loop = tornado.ioloop.IOLoop.instance()
    tornado.ioloop.PeriodicCallback(noTest, 1000, io_loop=loop).start()
    loop.start()


