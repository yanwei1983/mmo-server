MESSAGE(STATUS "cmake: socket_service")

# 搜索当前目录下所有文件
file(GLOB_RECURSE SRCS "*.cc" "*.cpp" "*.c" "*.h" "*.hpp")



# 生成库文件
ADD_LIBRARY(socket_service SHARED ${SRCS})
#SET_TARGET_PROPERTIES(socket_service PROPERTIES CLEAN_DIRECT_OUTPUT 1)
#SET_TARGET_PROPERTIES(socket_service PROPERTIES VERSION 0.1 SOVERSION 1)
set_target_properties(socket_service PROPERTIES CXX_VISIBILITY_PRESET hidden)



#指定库输出路径
SET(LIBRARY_OUTPUT_PATH ${LIBRARIES})
ADD_DEPENDENCIES(socket_service basecode network service_comm)
# 指定额外的链接库
TARGET_LINK_LIBRARIES(socket_service
 libbasecode.so 
 libservice_comm.so
 libnetwork.so 
 pthread
 rt
 dl
 ${JEMALLOC_LIB_NAME}
)
cotire(socket_service)
