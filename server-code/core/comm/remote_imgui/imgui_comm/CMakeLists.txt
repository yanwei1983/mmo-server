
MESSAGE(STATUS "cmake: remote_imgui_comm")
# 搜索当前目录下所有文件
file(GLOB_RECURSE SRCS "*.cc" "*.cpp" "*.c" "*.h" "*.hpp")

set(CMAKE_CXX_CLANG_TIDY "")

# 生成imgui.proto
PROTOBUF_GENERATE_CPP_NEW(IMGUI_PROTO_SRC IMGUI_PROTO_HEADER ${CMAKE_CURRENT_SOURCE_DIR}  ${CMAKE_CURRENT_SOURCE_DIR}/imgui.proto)

source_group_by_dir(SRCS)
# 生成库文件
ADD_LIBRARY(remote_imgui_comm STATIC ${SRCS} ${IMGUI_PROTO_SRC})



#指定库输出路径
SET(LIBRARY_OUTPUT_PATH ${LIBRARIES_CORE})
#ADD_DEPENDENCIES(remote_imgui_comm)

# 指定额外的链接库
TARGET_LINK_LIBRARIES(remote_imgui_comm 
 pb_util
 protobuf::libprotobuf
 ${JEMALLOC_LIB_NAME}
)

