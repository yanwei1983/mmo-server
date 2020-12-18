
MESSAGE(STATUS "cmake: render_client")
# 搜索当前目录下所有文件
file(GLOB_RECURSE SRCS "*.cc" "*.cpp" "*.c" "*.h" "*.hpp")



include_directories(SYSTEM ./glfw/include)

LINK_DIRECTORIES(./glfw/lib-vc2019)


# 生成库文件
add_executable(render_client ${SRCS})

#指定库输出路径
SET(EXECUTABLE_OUTPUT_PATH ${BINARIES})
#ADD_DEPENDENCIES(render_client)

# 指定额外的链接库
TARGET_LINK_LIBRARIES(render_client
 remote_imgui_comm
 glfw3.lib
 opengl32
)

