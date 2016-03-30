SET(jsoncpp_SRCS
  jsoncpp.cpp   
  )

ADD_LIBRARY(jsoncpp ${jsoncpp_SRCS})
INSTALL(TARGETS jsoncpp DESTINATION lib)

SET(jsoncpp_HEADERS
  json/json.h
  json/json-forwards.h
  )

INSTALL(FILES ${jsoncpp_HEADERS} DESTINATION include/jsoncpp/json)
