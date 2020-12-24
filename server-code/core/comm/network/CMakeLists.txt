MESSAGE(STATUS "cmake: network")

# 搜索当前目录下所有文件
file(GLOB_RECURSE SRCS "*.cc" "*.cpp" "*.c" "*.h" "*.hpp")




# 生成库文件
ADD_LIBRARY(network SHARED ${SRCS})
#SET_TARGET_PROPERTIES(network PROPERTIES CLEAN_DIRECT_OUTPUT 1)
#SET_TARGET_PROPERTIES(network PROPERTIES VERSION 0.1 SOVERSION 1)


#指定库输出路径
SET(LIBRARY_OUTPUT_PATH ${LIBRARIES_CORE})
ADD_DEPENDENCIES(network basecode crypto_comm)

# 指定额外的链接库
IF(WIN32)
TARGET_LINK_LIBRARIES(network
 PRIVATE crypto_comm
 PRIVATE liblz4_static
 PRIVATE event
 PRIVATE event_core
 PRIVATE event_extra
 PRIVATE ws2_32.lib
 PUBLIC basecode
 PUBLIC protobuf::libprotobuf
 PRIVATE ${JEMALLOC_LIB_NAME}
)
ELSE()
TARGET_LINK_LIBRARIES(network
 PRIVATE crypto_comm
 PRIVATE lz4
 PRIVATE libevent.so
 PRIVATE libevent_core.so
 PRIVATE libevent_pthreads.so
 PRIVATE libevent_extra.so
 PUBLIC libbasecode.so
 PUBLIC protobuf
 PRIVATE ${JEMALLOC_LIB_NAME}
)
ENDIF()

cotire(network)

