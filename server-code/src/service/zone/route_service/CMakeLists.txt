MESSAGE(STATUS "cmake: route_service")

# 搜索当前目录下所有文件
file(GLOB_RECURSE SRCS "*.cc" "*.cpp" "*.c" "*.h" "*.hpp")
source_group_by_dir(SRCS)




# 生成库文件
ADD_LIBRARY(route_service SHARED ${SRCS})
#SET_TARGET_PROPERTIES(route_service PROPERTIES CLEAN_DIRECT_OUTPUT 1)
#SET_TARGET_PROPERTIES(route_service PROPERTIES VERSION 0.1 SOVERSION 1)
set_target_properties(route_service PROPERTIES CXX_VISIBILITY_PRESET hidden)


#指定库输出路径
SET(LIBRARY_OUTPUT_PATH ${LIBRARIES})
ADD_DEPENDENCIES(route_service basecode network mydb myproto mydbtable service_comm gamemap)
# 指定额外的链接库
TARGET_LINK_LIBRARIES(route_service
 libbasecode.so 
 libmydb.so 
 libmyproto.so 
 libmydbtable.so 
 libservice_comm.so  
 libgamemap.so
 ${LIB_PTHREAD}
  ${JEMALLOC_LIB_NAME}
)

cotire(route_service)