MESSAGE(STATUS "cmake: game_event")

# 搜索当前目录下所有文件
file(GLOB_RECURSE SRCS "*.cc" "*.cpp" "*.c" "*.h" "*.hpp")




# 生成库文件
ADD_LIBRARY(game_event SHARED ${SRCS})
#SET_TARGET_PROPERTIES(game_event PROPERTIES CLEAN_DIRECT_OUTPUT 1)
#SET_TARGET_PROPERTIES(game_event PROPERTIES VERSION 0.1 SOVERSION 1)


#指定库输出路径
SET(LIBRARY_OUTPUT_PATH ${LIBRARIES_CORE})
ADD_DEPENDENCIES(game_event basecode)
# 指定额外的链接库
TARGET_LINK_LIBRARIES(game_event
 PRIVATE libevent.so
 PRIVATE libevent_core.so
 PRIVATE libevent_pthreads.so
 PRIVATE libevent_extra.so
 PUBLIC libbasecode.so
 PRIVATE ${JEMALLOC_LIB_NAME}
)
cotire(game_event)

