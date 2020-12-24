MESSAGE(STATUS "cmake: db_check")
# 搜索当前目录下所有文件
file(GLOB_RECURSE SRCS "*.cc" "*.cpp" "*.c" "*.h" "*.hpp")


# 生成库文件
add_executable(db_check ${SRCS})

#指定库输出路径
SET(EXECUTABLE_OUTPUT_PATH ${BINARIES})

# 指定额外的链接库

ADD_DEPENDENCIES(db_check basecode mydb mydbtable)
# 指定额外的链接库
TARGET_LINK_LIBRARIES(db_check
 libbasecode.so
 libmydb.so
 libmysqlclient.a
 libmydbtable.so
 crypto_comm
 ssl
 crypto
 z
 ${JEMALLOC_LIB_NAME}
 ${LIB_PTHREAD}
)

cotire(db_check)
