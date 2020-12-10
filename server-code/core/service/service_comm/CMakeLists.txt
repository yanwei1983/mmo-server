MESSAGE(STATUS "cmake: service_comm")

# 搜索当前目录下所有文件
file(GLOB_RECURSE SRCS "*.cc" "*.cpp" "*.c" "*.h" "*.hpp")





# 生成库文件
ADD_LIBRARY(service_comm SHARED ${SRCS})
#SET_TARGET_PROPERTIES(network PROPERTIES CLEAN_DIRECT_OUTPUT 1)
#SET_TARGET_PROPERTIES(network PROPERTIES VERSION 0.1 SOVERSION 1)


#指定库输出路径
SET(LIBRARY_OUTPUT_PATH ${LIBRARIES_CORE})
ADD_DEPENDENCIES(service_comm network basecode myproto mydb message_route game_event)
# 指定额外的链接库
TARGET_LINK_LIBRARIES(service_comm
 PUBLIC libnetwork.so
 PUBLIC libbasecode.so
 PUBLIC libmyproto.so
 PUBLIC libmydb.so
 PUBLIC libmessage_route.so
 PUBLIC libgame_event.so
 PUBLIC crypto_comm
 pthread
 rt
 dl
 ${JEMALLOC_LIB_NAME}
)
cotire(service_comm)
