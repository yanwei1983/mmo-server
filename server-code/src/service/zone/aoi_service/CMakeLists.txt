MESSAGE(STATUS "cmake: aoi_service")

# 搜索当前目录下所有文件
file(GLOB_RECURSE SRCS "*.cc" "*.cpp" "*.c" "*.h" "*.hpp")
file(GLOB_RECURSE HEADERS "*.h" "*.hpp")
file(GLOB_RECURSE SHARES "../server_share/*.h" "../server_share/*.hpp")

list(APPEND SRCS ${SHARES})
source_group_by_dir(SRCS)


# 生成库文件
ADD_LIBRARY(aoi_service SHARED ${SRCS})
#SET_TARGET_PROPERTIES(aoi_service PROPERTIES CLEAN_DIRECT_OUTPUT 1)
#SET_TARGET_PROPERTIES(aoi_service PROPERTIES VERSION 0.1 SOVERSION 1)
set_target_properties(aoi_service PROPERTIES CXX_VISIBILITY_PRESET hidden)



#指定库输出路径
SET(LIBRARY_OUTPUT_PATH ${LIBRARIES})
ADD_DEPENDENCIES(aoi_service basecode network mydb myproto service_comm gamemap)
# 指定额外的链接库
TARGET_LINK_LIBRARIES(aoi_service
 libbasecode.so
 libmydb.so 
 libmyproto.so 
 libservice_comm.so 
 libgamemap.so 
 pthread
 rt
 dl
 ${JEMALLOC_LIB_NAME}
)
cotire(aoi_service)
