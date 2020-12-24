MESSAGE(STATUS "cmake: crypto_comm")

# 搜索当前目录下所有文件
file(GLOB_RECURSE SRCS "*.cc" "*.cpp" "*.c" "*.h" "*.hpp")

set(CMAKE_CXX_CLANG_TIDY "")



# 生成库文件
ADD_LIBRARY(crypto_comm_static STATIC ${SRCS})

ADD_LIBRARY(crypto_comm SHARED ${SRCS})
#SET_TARGET_PROPERTIES(crypto_comm PROPERTIES CLEAN_DIRECT_OUTPUT 1)
#SET_TARGET_PROPERTIES(crypto_comm PROPERTIES VERSION 0.1 SOVERSION 1)


#指定库输出路径
SET(LIBRARY_OUTPUT_PATH ${LIBRARIES_CORE})
# 指定额外的链接库
IF(WIN32)
TARGET_LINK_LIBRARIES(crypto_comm_static
LINK_PRIVATE cryptopp-static
)
TARGET_LINK_LIBRARIES(crypto_comm
LINK_PRIVATE cryptopp-static
)

ELSE()
TARGET_LINK_LIBRARIES(crypto_comm_static
LINK_PRIVATE libcryptopp.a
)
TARGET_LINK_LIBRARIES(crypto_comm
LINK_PRIVATE cryptopp
)

ENDIF()
