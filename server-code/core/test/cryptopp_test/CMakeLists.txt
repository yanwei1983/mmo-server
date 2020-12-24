MESSAGE(STATUS "cmake: cryptopp_test")

# 搜索当前目录下所有文件
file(GLOB_RECURSE SRCS "*.cc" "*.cpp" "*.c" "*.h" "*.hpp")

set(CMAKE_CXX_CLANG_TIDY "")
# 生成库文件
add_executable(cryptopp_test ${SRCS})



#指定库输出路径
SET(EXECUTABLE_OUTPUT_PATH ${BINARIES_CORE})
ADD_DEPENDENCIES(cryptopp_test crypto_comm_static)
# 指定额外的链接库
TARGET_LINK_LIBRARIES(cryptopp_test
 crypto_comm_static
 ${JEMALLOC_LIB_NAME}
 ${LIB_PTHREAD}
)

