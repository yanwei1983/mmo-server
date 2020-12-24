MESSAGE(STATUS "cmake: mydb")
 
# 搜索当前目录下所有文件
file(GLOB_RECURSE SRCS "*.cc" "*.cpp" "*.c" "*.h" "*.hpp")


# 生成库文件
ADD_LIBRARY(mydb SHARED ${SRCS})


#指定库输出路径
SET(LIBRARY_OUTPUT_PATH ${LIBRARIES_CORE})
ADD_DEPENDENCIES(mydb basecode)
# 指定额外的链接库
TARGET_LINK_LIBRARIES(mydb
	PUBLIC libbasecode.so
	PRIVATE libmysqlclient.a
	PRIVATE ssl
	PUBLIC ${LIB_PTHREAD}
	PRIVATE ${JEMALLOC_LIB_NAME}
	PRIVATE dl
)
cotire(mydb)
