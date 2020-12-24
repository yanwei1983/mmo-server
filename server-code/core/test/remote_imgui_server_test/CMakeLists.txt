
MESSAGE(STATUS "cmake: remote_imgui_server_test")
# 搜索当前目录下所有文件
file(GLOB_RECURSE SRCS "*.cc" "*.cpp" "*.c" "*.h" "*.hpp")


# 生成库文件
add_executable(remote_imgui_server_test ${SRCS})

#指定库输出路径
SET(EXECUTABLE_OUTPUT_PATH ${BINARIES_CORE})
ADD_DEPENDENCIES(remote_imgui_server_test remote_imgui_server)

# 指定额外的链接库
TARGET_LINK_LIBRARIES(remote_imgui_server_test
 remote_imgui_server
)

