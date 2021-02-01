
MESSAGE(STATUS "cmake: bttree_editor")
# 搜索当前目录下所有文件
file(GLOB_RECURSE SRCS "*.cc" "*.cpp" "*.c" "*.h" "*.hpp")

source_group_by_dir(SRCS)

include_directories(SYSTEM ./glfw/include)

LINK_DIRECTORIES(./glfw/lib-vc2019)


# 生成库文件
add_executable(bttree_editor ${SRCS})

#指定库输出路径
SET(EXECUTABLE_OUTPUT_PATH ${BINARIES_CORE})
#ADD_DEPENDENCIES(bttree_editor)

# 指定额外的链接库
TARGET_LINK_LIBRARIES(bttree_editor
 imgui
 basecode
 fmt
 glfw3.lib
 opengl32
)

