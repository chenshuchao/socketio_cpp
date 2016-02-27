SET(engineio_SRCS
  packet.cc
  parser.cc
  server.cc
  socket.cc
  transport.cc
  transports/polling.cc
  transports/polling_xhr.cc
  transports/polling_jsonp.cc
  transports/websocket.cc
  )

ADD_LIBRARY(engineio ${engineio_SRCS})

TARGET_LINK_LIBRARIES(engineio woody_http woody_websocket woody_base muduo_net muduo_base pthread)
