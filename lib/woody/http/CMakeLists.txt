SET(woody_http_SRCS
  http_codec.cc
  http_handler.cc
  http_request.cc
  http_response.cc
  http_server.cc
  )

ADD_LIBRARY(woody_http ${woody_http_SRCS})
TARGET_LINK_LIBRARIES(woody_http woody_http_parser)
