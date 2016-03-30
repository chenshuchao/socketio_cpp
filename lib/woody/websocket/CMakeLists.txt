SET(woody_websocket_SRCS
  websocket_codec.cc
  websocket_frame.cc
  websocket_handler.cc
  websocket_message.cc
  websocket_parser.cc
  #websocket_server.cc
  )

ADD_LIBRARY(woody_websocket ${woody_websocket_SRCS})
TARGET_LINK_LIBRARIES(woody_websocket muduo_net)
INSTALL(TARGETS woody_websocket DESTINATION lib)

