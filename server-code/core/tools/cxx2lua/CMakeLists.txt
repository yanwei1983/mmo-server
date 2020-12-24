MESSAGE(STATUS "cmake: cxx2lua")

include(FindLibClang)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${LIBCLANG_CXXFLAGS}")
LINK_DIRECTORIES(${LIBCLANG_LIBDIR})

# 搜索当前目录下所有文件
file(GLOB_RECURSE SRCS "*.cc" "*.cpp" "*.c" "*.h" "*.hpp")

add_definitions (-Wno-format-security)


# 生成库文件
add_executable(cxx2lua ${SRCS})

#指定库输出路径
SET(EXECUTABLE_OUTPUT_PATH ${BINARIES_CORE})

# 指定额外的链接库
TARGET_LINK_LIBRARIES(cxx2lua
 fmt
 libclang.so
 ${LIB_PTHREAD}
)


cotire(cxx2lua)
