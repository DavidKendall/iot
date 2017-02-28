import tornado.httpserver
import tornado.websocket
import tornado.ioloop
import tornado.web
import os
import logging
import datetime
import json
topics = {}


class WSHandler(tornado.websocket.WebSocketHandler):

          
  def check_origin(self, origin):
    return True

  def open(self, chan, mode):
    logging.warning('New client connection')

  def on_message(self, message):
    global topics

    data = json.loads(message.decode('utf8'))
    messageType = data[u'type']
    topic = data[u'id']
    if messageType == u'SUBSCRIBE':
      if topic not in topics:
        topics[topic] = {'subscribers': [self], 
                         'publisher': None, 
                         'last_published': None}
      else:
        topics[topic]['subscribers'].append(self)
      logging.warning('New subscriber to topic {}'.format(topic))
    elif messageType == u'DATA':
      if topic not in topics:
        topics[topic] = {'subscribers': [],
                         'publisher' : None,
                         'last_published': datetime.datetime.now()}
        logging.warning('New topic {}'.format(topic))
      if not topics[topic]['publisher']:
        topics[topic]['publisher'] = self
        logging.warning('New publisher')
      else:
        topics[topic]['last_published'] = datetime.datetime.now()
      for client in topics[topic]['subscribers']:
        try:
          client.write_message(message)
        except tornado.websocket.WebSocketClosedError:
          topics[topic]['subscribers'].remove(client)
    elif messageType == u'COMMAND':
      if topics[topic]['publisher']:
        topics[topic]['publisher'].write_message(message)
    else:
      logging.warning('Unknown message type')

  def on_close(self):
    global topics

    for topic in topics:
      if self == topics[topic]['publisher']:
        topics[topic]['publisher'] = None
    logging.warning('Client closing')

settings = {'auto_reload': True}

application = tornado.web.Application([
    (r'/ws/(.*)/(.*)', WSHandler),
], **settings)


# def test():
#   for k, v in publishers.items():
#     delta = datetime.datetime.now() - v['lastact']
#     if delta.seconds > 1:
#       logging.warning(
#           "Callback. Client %s did not send data in the last 2 secs. Closing" % k)
#       del publishers[k]
#       v['stream'].stream.close()
#
#
# def noTest():
#   pass

if __name__ == "__main__":
  http_server = tornado.httpserver.HTTPServer(application)
  http_server.listen(9000)
  loop = tornado.ioloop.IOLoop.instance()
  # tornado.ioloop.PeriodicCallback(noTest, 1000, io_loop=loop).start()
  loop.start()
