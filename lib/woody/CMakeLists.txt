ADD_SUBDIRECTORY(base)
ADD_SUBDIRECTORY(lib)
ADD_SUBDIRECTORY(http)
ADD_SUBDIRECTORY(websocket)

#TARGET_LINK_LIBRARIES(woody woody_websocket woody_http woody_base muduo_net muduo_base pthread)
