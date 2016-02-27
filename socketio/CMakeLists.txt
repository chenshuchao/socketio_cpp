SET(socketio_SRCS
  client.cc
  namespace.cc
  packet.cc
  parser.cc
  server.cc
  socket.cc
    )

ADD_LIBRARY(socketio ${socketio_SRCS})

TARGET_LINK_LIBRARIES(socketio engineio)
