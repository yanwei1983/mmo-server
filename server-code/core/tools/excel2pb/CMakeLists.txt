MESSAGE(STATUS "cmake: excel2pb")


# 搜索当前目录下所有文件
file(GLOB_RECURSE SRCS "*.cc" "*.cpp" "*.c" "*.h" "*.hpp")
set(CMAKE_CXX_CLANG_TIDY "")
# 生成库文件
add_executable(excel2pb ${SRCS})

#指定库输出路径
SET(EXECUTABLE_OUTPUT_PATH ${BINARIES_CORE})

ADD_DEPENDENCIES(excel2pb pb_util data_pack_static)
# 指定额外的链接库
TARGET_LINK_LIBRARIES(excel2pb
 xlnt
 protobuf
 pb_util
 libfmt.a
 libdata_pack_static.a
 ${LIB_PTHREAD}
)


cotire(excel2pb)
