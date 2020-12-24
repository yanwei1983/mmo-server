MESSAGE(STATUS "cmake: db_tester")

# 搜索当前目录下所有文件
file(GLOB_RECURSE SRCS "*.cc" "*.cpp" "*.c" "*.h" "*.hpp")

set(CMAKE_CXX_CLANG_TIDY "")
# 生成库文件
add_executable(db_tester ${SRCS})



#指定库输出路径
SET(EXECUTABLE_OUTPUT_PATH ${BINARIES_CORE})
ADD_DEPENDENCIES(db_tester basecode mydb mydbtable)
# 指定额外的链接库
TARGET_LINK_LIBRARIES(db_tester
 libbasecode.so
 libmydb.so
 libmysqlclient.a
 libmydbtable.so
 ssl
 crypto
 z
 ${JEMALLOC_LIB_NAME}
 ${LIB_PTHREAD}
)





