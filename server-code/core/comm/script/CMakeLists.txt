MESSAGE(STATUS "cmake: script")
 
# 搜索当前目录下所有文件
file(GLOB_RECURSE SRCS "*.cc" "*.cpp" "*.c" "*.h" "*.hpp")

source_group_by_dir(SRCS)





SET(BASECODE2LUA_SRC 
"BaseCode.h"
"BaseType.h"
"FileLock.h"
"FileUtil.h"
"MD5.h"
"MyTimer.h"
"RandomGet.h"
"TimeUtil.h"
"AxisAlignedBox.h"
"GameMath.h"
"MathDef.h"
"Matrix3.h"
"Matrix4.h"
"Quaternion.h"
"Ray.h"
"Vector2.h"
"Vector3.h"
"ScriptManager.h"
)

GET_DIRECTORY_PROPERTY(includes INCLUDE_DIRECTORIES)
# make basecode2lua 生成 basecode2lua.cpp
add_custom_target(basecode2lua
COMMAND echo ${BASECODE2LUA_SRC} > ${CMAKE_CURRENT_BINARY_DIR}/basecode_cpps.txt && echo ${includes} > ${CMAKE_CURRENT_BINARY_DIR}/basecode_includes.txt 
COMMAND cxx2lua --cpps=${CMAKE_CURRENT_BINARY_DIR}/basecode_cpps.txt --include=${CMAKE_CURRENT_BINARY_DIR}/basecode_includes.txt 
--output=${CMAKE_CURRENT_BINARY_DIR}/basecode2lua.cpp --output_functionname=basecode2lua --skip_cpp --format=clang-format-9
COMMENT "gen basecode for export_lua"
DEPENDS cxx2lua
 )

# # # basecode2lua 挂接到 make gen_all_lua
ADD_DEPENDENCIES(gen_all_lua basecode2lua)

# basecode2lua.cpp不存在，则生成 basecode2lua.cpp
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/basecode2lua.cpp
COMMAND echo ${BASECODE2LUA_SRC} > ${CMAKE_CURRENT_BINARY_DIR}/basecode_cpps.txt && echo ${includes} > ${CMAKE_CURRENT_BINARY_DIR}/basecode_includes.txt 
COMMAND cxx2lua --cpps=${CMAKE_CURRENT_BINARY_DIR}/basecode_cpps.txt --include=${CMAKE_CURRENT_BINARY_DIR}/basecode_includes.txt 
--output=${CMAKE_CURRENT_BINARY_DIR}/basecode2lua.cpp --output_functionname=basecode2lua --skip_cpp --format=clang-format-9
COMMENT "gen basecode for basecode2lua.cpp"
  DEPENDS cxx2lua basecode
)

set_property (SOURCE ${CMAKE_CURRENT_BINARY_DIR}/basecode2lua.cpp PROPERTY COTIRE_EXCLUDED  "TRUE")

#指定库输出路径
SET(LIBRARY_OUTPUT_PATH ${LIBRARIES})
# 生成库文件
ADD_LIBRARY(script SHARED ${SRCS})
add_dependencies(script basecode)

# 指定额外的链接库
TARGET_LINK_LIBRARIES(script
  liblua.a
  libbasecode.so
  ${JEMALLOC_LIB_NAME}
)

cotire(script)
target_sources(script PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/basecode2lua.cpp)