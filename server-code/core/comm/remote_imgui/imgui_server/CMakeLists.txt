
MESSAGE(STATUS "cmake: remote_imgui_server")
# 搜索当前目录下所有文件
file(GLOB_RECURSE SRCS "*.cc" "*.cpp" "*.c" "*.h" "*.hpp")

# 生成库文件
add_library(remote_imgui_server SHARED ${SRCS})


#指定库输出路径
SET(LIBRARY_OUTPUT_PATH ${LIBRARIES_CORE})
ADD_DEPENDENCIES(remote_imgui_server remote_imgui_comm network)

# 指定额外的链接库
TARGET_LINK_LIBRARIES(remote_imgui_server 
 PUBLIC remote_imgui_comm
 PUBLIC network
 PUBLIC ${JEMALLOC_LIB_NAME}
 PUBLIC ${LIB_PTHREAD}
)

