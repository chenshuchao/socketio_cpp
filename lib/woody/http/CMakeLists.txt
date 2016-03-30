SET(woody_http_SRCS
  http_codec.cc
  http_handler.cc
  http_request.cc
  http_response.cc
  http_server.cc
  )

ADD_LIBRARY(woody_http ${woody_http_SRCS} $<TARGET_OBJECTS:woody_http_parser>) 
TARGET_LINK_LIBRARIES(woody_http muduo_net)
INSTALL(TARGETS woody_http DESTINATION lib)

SET(woody_http_HEADERS
  http_application.h
  http_request.h
  http_response.h
  http_server.h
  )
INSTALL(FILES ${woody_http_HEADERS} DESTINATION include/woody/http)
