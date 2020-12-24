MESSAGE(STATUS "cmake: sql2cpp")
# 搜索当前目录下所有文件
file(GLOB_RECURSE SRCS "*.cc" "*.cpp" "*.c" "*.h" "*.hpp")


# 生成库文件
add_executable(sql2cpp ${SRCS})

#指定库输出路径
SET(EXECUTABLE_OUTPUT_PATH ${BINARIES_CORE})

# 指定额外的链接库
TARGET_LINK_LIBRARIES(sql2cpp
 libfmt.a
 ${LIB_PTHREAD}
)


cotire(sql2cpp)
