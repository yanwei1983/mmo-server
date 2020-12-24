MESSAGE(STATUS "cmake: protoc-gen-sql")


# 搜索当前目录下所有文件
file(GLOB SRCS "*.cc" "*.cpp" "*.c" "*.h" "*.hpp")



# 生成库文件
add_executable(protoc-gen-sql ${SRCS} )
ADD_DEPENDENCIES(protoc-gen-sql sql_options_static)
#指定库输出路径
SET(EXECUTABLE_OUTPUT_PATH ${BINARIES_CORE})
# 指定额外的链接库
TARGET_LINK_LIBRARIES(protoc-gen-sql
 protoc
 protobuf
 libsql_options_static.a
 fmt
 ${LIB_PTHREAD}
)
#cotire(protoc-gen-sql)


