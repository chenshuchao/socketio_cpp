SET(socketio_SRCS
  adapter.cc
  client.cc
  namespace.cc
  packet.cc
  parser.cc
  server.cc
  socket.cc
    )

ADD_LIBRARY(socketio ${socketio_SRCS})

TARGET_LINK_LIBRARIES(socketio engineio)

INSTALL(TARGETS socketio DESTINATION lib)

SET(socketio_HEADERS
  adapter.h
  callback.h
  namespace.h
  server.h
  socket.h
)

INSTALL(FILES ${socketio_HEADERS} DESTINATION include/socketio)
