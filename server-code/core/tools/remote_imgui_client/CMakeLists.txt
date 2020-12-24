
MESSAGE(STATUS "cmake: remote_imgui_client")
# 搜索当前目录下所有文件
file(GLOB_RECURSE SRCS "*.cc" "*.cpp" "*.c" "*.h" "*.hpp")



include_directories(SYSTEM ./glfw/include)

LINK_DIRECTORIES(./glfw/lib-vc2019)


# 生成库文件
add_executable(remote_imgui_client ${SRCS})

#指定库输出路径
SET(EXECUTABLE_OUTPUT_PATH ${BINARIES_CORE})
#ADD_DEPENDENCIES(remote_imgui_client)

# 指定额外的链接库
TARGET_LINK_LIBRARIES(remote_imgui_client
 remote_imgui_comm
 basecode
 fmt
 network
 protobuf::libprotobuf
 glfw3.lib
 opengl32
)

