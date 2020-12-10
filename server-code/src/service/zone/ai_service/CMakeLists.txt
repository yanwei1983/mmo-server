MESSAGE(STATUS "cmake: ai_service")

# 搜索当前目录下所有文件
file(GLOB_RECURSE SRCS "*.cc" "*.cpp" "*.c" "*.h" "*.hpp")
file(GLOB_RECURSE HEADERS "*.h" "*.hpp")
file(GLOB_RECURSE SHARES "../server_share/*.h" "../server_share/*.hpp")

list(APPEND SRCS ${SHARES})
source_group_by_dir(SRCS)





GET_DIRECTORY_PROPERTY(includes INCLUDE_DIRECTORIES)


# make ai2lua 生成 ai2lua.cpp
add_custom_target(ai2lua
COMMAND echo ${HEADERS} > ${CMAKE_CURRENT_BINARY_DIR}/ai_cpps.txt && echo ${includes} > ${CMAKE_CURRENT_BINARY_DIR}/ai_includes.txt 
COMMAND ${BINARIES_CORE}/cxx2lua --cpps=${CMAKE_CURRENT_BINARY_DIR}/ai_cpps.txt --include=${CMAKE_CURRENT_BINARY_DIR}/ai_includes.txt 
--output=${CMAKE_CURRENT_BINARY_DIR}/ai2lua.cpp --output_functionname=ai2lua --skip_cpp --format=clang-format-9
COMMENT "gen ai2lua for export_lua"
DEPENDS cxx2lua
 )

# # # 将ai2lua 挂接到 make gen_all_lua
ADD_DEPENDENCIES(gen_all_lua ai2lua)

# ai2lua.cpp不存在，则生成 ai2lua.cpp
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/ai2lua.cpp
COMMAND echo ${HEADERS} > ${CMAKE_CURRENT_BINARY_DIR}/ai_cpps.txt && echo ${includes} > ${CMAKE_CURRENT_BINARY_DIR}/ai_includes.txt 
COMMAND ${BINARIES_CORE}/cxx2lua --cpps=${CMAKE_CURRENT_BINARY_DIR}/ai_cpps.txt --include=${CMAKE_CURRENT_BINARY_DIR}/ai_includes.txt 
--output=${CMAKE_CURRENT_BINARY_DIR}/ai2lua.cpp --output_functionname=ai2lua --skip_cpp --format=clang-format-9
COMMENT "gen ai2lua for ai2lua.cpp"
  DEPENDS cxx2lua ${HEADERS}
)

set_property (SOURCE ${CMAKE_CURRENT_BINARY_DIR}/ai2lua.cpp PROPERTY COTIRE_EXCLUDED "TRUE")

# 生成库文件
ADD_LIBRARY(ai_service SHARED ${SRCS} ${CMAKE_CURRENT_BINARY_DIR}/ai2lua.cpp)
#SET_TARGET_PROPERTIES(ai_service PROPERTIES CLEAN_DIRECT_OUTPUT 1)
#SET_TARGET_PROPERTIES(ai_service PROPERTIES VERSION 0.1 SOVERSION 1)
set_target_properties(ai_service PROPERTIES CXX_VISIBILITY_PRESET hidden)



#指定库输出路径
SET(LIBRARY_OUTPUT_PATH ${LIBRARIES})
ADD_DEPENDENCIES(ai_service basecode network mydb myproto mydbtable service_comm gamemap script)
# 指定额外的链接库
TARGET_LINK_LIBRARIES(ai_service
 libbasecode.so
 libmydb.so 
 libmyproto.so 
 libmydbtable.so 
 libservice_comm.so 
 libgamemap.so 
 libscript.so
 pthread
 rt
 dl
 ${JEMALLOC_LIB_NAME}
)
cotire(ai_service)
target_sources(ai_service PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/ai2lua.cpp)