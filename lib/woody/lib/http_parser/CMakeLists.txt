SET(http_parser_SRCS
  http_parser.c
  )

ADD_LIBRARY(woody_http_parser ${http_parser_SRCS})
