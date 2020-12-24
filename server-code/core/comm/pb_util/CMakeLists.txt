MESSAGE(STATUS "cmake: pb_util")





	
# 搜索当前目录下所有文件
file(GLOB_RECURSE SRC "*.cc" "*.cpp" "*.c" "*.h" "*.hpp")

# 生成库文件
ADD_LIBRARY(pb_util STATIC ${SRC})

#指定库输出路径
SET(LIBRARY_OUTPUT_PATH ${LIBRARIES_CORE})

# 指定额外的链接库
TARGET_LINK_LIBRARIES(pb_util
	fmt
	protobuf::libprotobuf
	${LIB_PTHREAD}
) 
 
cotire(pb_util)