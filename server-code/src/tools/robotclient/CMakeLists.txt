MESSAGE(STATUS "cmake: robotclient")
# 搜索当前目录下所有文件
file(GLOB_RECURSE SRCS "*.cc" "*.cpp" "*.c" "*.h" "*.hpp")



# 生成库文件
add_executable(robotclient ${SRCS})



#指定库输出路径
SET(EXECUTABLE_OUTPUT_PATH ${BINARIES})
ADD_DEPENDENCIES(robotclient basecode pb_util crypto_comm network script myproto game_event)

# 指定额外的链接库
TARGET_LINK_LIBRARIES(robotclient
 libevent.so
 libevent_core.so
 libevent_pthreads.so
 libevent_extra.so
 libbasecode.so
 libnetwork.so
 libgame_event.so
 libscript.so
 libmyproto.so
 crypto_comm
 pb_util
 protobuf
 ${JEMALLOC_LIB_NAME}
 ${LIB_PTHREAD}
)
cotire(robotclient)
