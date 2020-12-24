MESSAGE(STATUS "cmake: pbbin2json")
# 搜索当前目录下所有文件
file(GLOB_RECURSE SRCS "*.cc" "*.cpp" "*.c" "*.h" "*.hpp")
set(CMAKE_CXX_CLANG_TIDY "")
# 生成库文件
add_executable(pbbin2json ${SRCS})

#指定库输出路径
SET(EXECUTABLE_OUTPUT_PATH ${BINARIES_CORE})

ADD_DEPENDENCIES(pbbin2json pb_util)
# 指定额外的链接库
TARGET_LINK_LIBRARIES(pbbin2json
 protobuf
 pb_util
 ${LIB_PTHREAD}
)



cotire(pbbin2json)