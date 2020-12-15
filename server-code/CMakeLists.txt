cmake_minimum_required(VERSION 3.2)
MESSAGE(STATUS "cmake: project root")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(TIDY "ON for use clang-tidy(cmake version 3.6+) or OFF" OFF)
option(USE_LUADEBUGER "ON for use lua_debuger or OFF" OFF)
option(USE_JEMALLOC "ON for use jemalloc or OFF" ON)
option(USE_ASAN "ON for use Address Sanitizer or OFF" OFF)
option(USE_UBSAN "ON for use UB Sanitizer or OFF" OFF)
option(USE_TSAN "ON for use Thread Sanitizer or OFF" OFF)
option(USE_LIBCXX "ON for use libcxx or OFF" OFF)
option(USE_STATIC_LINK_STD "ON for use static link gcc or OFF" OFF)

set_directory_properties(PROPERTIES COTIRE_ADD_UNITY_BUILD FALSE)




PROJECT(Framework C CXX)


set (CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/core/CMake")
include(cotire)
set_directory_properties(PROPERTIES
    COTIRE_PREFIX_HEADER_INCLUDE_PATH "${PROJECT_SOURCE_DIR}/depends" "${PROJECT_SOURCE_DIR}/core/comm" "${PROJECT_SOURCE_DIR}/src/game_comm")


SET(CMAKE_CXX_ERROR_FLAGS 
"-fexceptions \
 -Wall \
 -Wno-reorder \
 -Wno-unused-variable \
 -Wno-unused-function \
 -Wno-unused-result \
 -Wno-undefined-internal \
 -Wno-deprecated \
 -Wno-unused-private-field \
 -Wno-nonportable-include-path \
 -Wno-undefined-var-template")

add_compile_options("-D__FILENAME__='\"$(subst ${CMAKE_SOURCE_DIR}/,,$(abspath $<))\"'")

SET(CMAKE_C_FLAGS_DEBUG "$ENV{CFLAGS} -g -pipe -D__DEBUG -DDEBUG -pthread")
SET(CMAKE_C_FLAGS_RELEASE "$ENV{CFLAGS} -g -pipe -O2 -DNDEBUG -pthread")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -std=c++17 -g -pipe -D__DEBUG -DDEBUG ${CMAKE_CXX_ERROR_FLAGS} -pthread")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -std=c++17 -g -pipe -O2 -DNDEBUG ${CMAKE_CXX_ERROR_FLAGS} -pthread")

add_compile_options("-fPIC")


#add_compile_options(-Wl,--unresolved-symbols=ignore-in-shared-libs)
IF(NOT CMAKE_CXX_COMPILER MATCHES "clang")
#gcc/vs
add_compile_options("-rdynamic")

ELSE()
#clang
  link_libraries("-Wl,--export-dynamic")
  find_program(LLVM_LLD_FOUND lld-9)
  if(LLVM_LLD_FOUND)
    link_libraries("-fuse-ld=lld-9")
  endif(LLVM_LLD_FOUND)
  
  find_program(LLVM_AR_FOUND llvm-ar-9)
  if(LLVM_AR_FOUND)
    SET(CMAKE_AR llvm-ar-9)
  endif(LLVM_AR_FOUND)
  find_program(LLVM_RANLIB_FOUND llvm-ranlib-9)
  if(LLVM_RANLIB_FOUND)
    SET(CMAKE_RANLIB llvm-ranlib-9)
  endif(LLVM_RANLIB_FOUND)

ENDIF()

IF(USE_STATIC_LINK_STD)
add_compile_options("-static-libstdc++")
add_compile_options("-static-libgcc")
ENDIF()

IF(USE_LIBCXX)
add_compile_options("-stdlib=libc++")
SET(LIB_FS "c++fs")
ELSE()
SET(LIB_FS "stdc++fs")
ENDIF()

macro(try_enable_malloc)

IF(USE_ASAN)
  add_compile_options("-DUSE_ASAN")
  add_compile_options("-fsanitize=address")
  add_compile_options("-fno-omit-frame-pointer")
  add_compile_options("-fsanitize-address-use-after-scope")
  add_compile_options("-fno-common")
  add_compile_options("-fsanitize-recover=address")
  link_libraries("-fsanitize=address")

  SET(USE_TSAN 0)
  SET(USE_JEMALLOC 0)
  message("asan:address")
ENDIF(USE_ASAN)


IF(USE_UBSAN)
  add_compile_options("-fsanitize=undefined")
  link_libraries("-fsanitize=undefined")
  message("asan:undefined")
  SET(USE_JEMALLOC 0)
ENDIF(USE_UBSAN)

IF(USE_TSAN)
  add_compile_options("-fsanitize=thread")
  link_libraries("-fsanitize=thread")
  message("asan:thread")
  SET(USE_JEMALLOC 0)
ENDIF(USE_TSAN)

IF(USE_JEMALLOC)
ADD_DEFINITIONS(-DUSE_JEMALLOC)
set(JEMALLOC_LIB_NAME "libjemalloc.so")
message("jemalloc:${JEMALLOC_LIB_NAME}")
ELSE()
ENDIF()

endmacro(try_enable_malloc)

#-----------------------------------------------------------------------------
# Enable ccache if not already enabled by symlink masquerading and if no other
# CMake compiler launchers are already defined
#-----------------------------------------------------------------------------
find_program(CCACHE_EXECUTABLE ccache)
mark_as_advanced(CCACHE_EXECUTABLE)
if(CCACHE_EXECUTABLE)
  foreach(LANG C CXX)
    if(NOT DEFINED CMAKE_${LANG}_COMPILER_LAUNCHER AND NOT CMAKE_${LANG}_COMPILER MATCHES ".*/ccache$")
      message(STATUS "Enabling ccache for ${LANG}")
      set(CMAKE_${LANG}_COMPILER_LAUNCHER ${CCACHE_EXECUTABLE} CACHE STRING "")
    endif()
  endforeach()
endif()

include(FindThreads)

set(PROTOBUF_USE_STATIC_LIBS ON)
set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} ${PROJECT_SOURCE_DIR}/depends/src/protobuf/src)
set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} ${PROJECT_SOURCE_DIR}/depends/lib)
SET(PROTOBUF_PROTOC_EXECUTABLE ${PROJECT_SOURCE_DIR}/depends/bin/protoc) 
include(FindProtobuf)
include(protobuf_gen)

set (MY_CLANG_TIDY_STR "clang-tidy-9")
IF(TIDY)
set(CMAKE_CXX_CLANG_TIDY ${MY_CLANG_TIDY_STR})
message("tidy:${MY_CLANG_TIDY_STR}")
ELSE()
ENDIF()



message(STATUS "CMAKE_CXX_COMPILER:" ${CMAKE_CXX_COMPILER})



SET(CMAKE_SKIP_RPATH  FALSE)
SET(CMAKE_SKIP_BUILD_RPATH  FALSE)
SET(CMAKE_SKIP_INSTALL_RPATH  FALSE)

SET(CMAKE_BUILD_RPATH "./:./libs:../libs:./depends:../depends")
SET(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE) 
SET(CMAKE_INSTALL_RPATH "./:./libs:../libs:./depends:../depends")
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH FALSE) 

include_directories(SYSTEM ${PROJECT_SOURCE_DIR}/depends/src/jemalloc/include)
include_directories(SYSTEM ${PROJECT_SOURCE_DIR}/depends/src/cpp_redis/includes)
include_directories(SYSTEM ${PROJECT_SOURCE_DIR}/depends/src/cpp_redis/tacopie/includes)
include_directories(SYSTEM ${PROJECT_SOURCE_DIR}/depends/src/libevent/include)
include_directories(SYSTEM ${PROJECT_SOURCE_DIR}/depends/src/protobuf/src)
include_directories(SYSTEM ${PROJECT_SOURCE_DIR}/depends/src/lua/src)
include_directories(SYSTEM ${PROJECT_SOURCE_DIR}/depends/src/curlpp/include)
include_directories(SYSTEM ${PROJECT_SOURCE_DIR}/depends/src/rapidjson/include)
include_directories(SYSTEM ${PROJECT_SOURCE_DIR}/depends/src/xlnt/include)
include_directories(SYSTEM ${PROJECT_SOURCE_DIR}/depends/src/fmt/include)
include_directories(SYSTEM ${PROJECT_SOURCE_DIR}/depends/src/brpc/include)
include_directories(SYSTEM ${PROJECT_SOURCE_DIR}/depends/src/gflags/include)
include_directories(SYSTEM ${PROJECT_SOURCE_DIR}/depends/src/leveldb/include)
include_directories(SYSTEM ${PROJECT_SOURCE_DIR}/depends/src/snappy)
#include_directories(SYSTEM ${PROJECT_SOURCE_DIR}/depends/src/libwebsockets/build/include)
include_directories(SYSTEM ${PROJECT_SOURCE_DIR}/depends/src/box2d/include)
include_directories(SYSTEM ${PROJECT_SOURCE_DIR}/depends/src/cpp-taskflow)
include_directories(SYSTEM ${PROJECT_SOURCE_DIR}/depends/src/recastnavigation/Recast/Inlcude)
include_directories(SYSTEM ${PROJECT_SOURCE_DIR}/depends/src/recastnavigation/Detour/Inlcude)
#include_directories(SYSTEM ${PROJECT_SOURCE_DIR}/depends/src/recastnavigation/DetourCrowd/Inlcude)
#include_directories(SYSTEM ${PROJECT_SOURCE_DIR}/depends/src/recastnavigation/DetourTileCache/Inlcude)
include_directories(SYSTEM ${PROJECT_SOURCE_DIR}/depends/src/spy/include)
include_directories(SYSTEM ${PROJECT_SOURCE_DIR}/depends/src/)


include_directories(${PROJECT_SOURCE_DIR}/core/comm/basecode/)
include_directories(${PROJECT_SOURCE_DIR}/core/comm/basecode/math)
include_directories(${PROJECT_SOURCE_DIR}/core/comm/pb_util/)
include_directories(${PROJECT_SOURCE_DIR}/core/comm/network/)
include_directories(${PROJECT_SOURCE_DIR}/core/comm/game_event/)
include_directories(${PROJECT_SOURCE_DIR}/core/comm/crypto_comm/)
include_directories(${PROJECT_SOURCE_DIR}/core/proto/)
include_directories(${PROJECT_SOURCE_DIR}/core/comm/message_route/)
include_directories(${PROJECT_SOURCE_DIR}/core/comm/message_route/db_proto)
include_directories(${PROJECT_SOURCE_DIR}/core/comm/script/)
include_directories(${PROJECT_SOURCE_DIR}/core/comm/mydb/)
include_directories(${PROJECT_SOURCE_DIR}/core/service/comm/)
include_directories(${PROJECT_SOURCE_DIR}/core/service/service_comm/)
include_directories(${PROJECT_SOURCE_DIR}/core/service/rpcservice_comm/)



macro(source_group_by_dir source_files)
    set(sgbd_cur_dir ${CMAKE_CURRENT_SOURCE_DIR})
    foreach(sgbd_file ${${source_files}})
        string(REGEX REPLACE ${sgbd_cur_dir}/\(.*\) \\1 sgbd_fpath ${sgbd_file})
        string(REGEX REPLACE "\(.*\)/.*" \\1 sgbd_group_name ${sgbd_fpath})
        string(COMPARE EQUAL ${sgbd_fpath} ${sgbd_group_name} sgbd_nogroup)
        string(REPLACE "\\" "/" sgbd_group_name ${sgbd_group_name})
        if(sgbd_nogroup)
            set(sgbd_group_name "")
        endif(sgbd_nogroup)	
        source_group(${sgbd_group_name} FILES ${sgbd_file})
        list(FIND sgbd_dirs "${sgbd_cur_dir}/${sgbd_group_name}" RET)
        if (${RET} EQUAL -1)
        list(APPEND sgbd_dirs "${sgbd_cur_dir}/${sgbd_group_name}")
        endif()
    endforeach(sgbd_file)
    foreach(sgbd_dir ${sgbd_dirs})
        include_directories("${sgbd_dir}")
        #message("add include dir:" "${sgbd_dir}")
    endforeach()	
endmacro(source_group_by_dir)

macro(source_group_by_files sgbd_group_name source_files)
	set(sgbd_cur_dir ${CMAKE_CURRENT_SOURCE_DIR})
    foreach(sgbd_file ${${source_files}})	
		message("group:${sgbd_group_name} file:${sgbd_file}")	
        source_group(${sgbd_group_name} FILES ${sgbd_file})
    endforeach(sgbd_file)
endmacro(source_group_by_files)




SET(LIBRARIES_CORE ${PROJECT_BINARY_DIR}/lib/core)
SET(BINARIES_CORE ${PROJECT_BINARY_DIR}/bin/core)


IF (CMAKE_BUILD_TYPE MATCHES "Debug")
SET(LIBRARIES ${PROJECT_BINARY_DIR}/lib/debug)
SET(BINARIES ${PROJECT_BINARY_DIR}/bin/debug)
ELSE()
SET(LIBRARIES ${PROJECT_BINARY_DIR}/lib/release)
SET(BINARIES ${PROJECT_BINARY_DIR}/bin/release)
ENDIF()

LINK_DIRECTORIES(
 ${PROJECT_SOURCE_DIR}/depends/lib/
 ${LIBRARIES_CORE}
 ${LIBRARIES}
)


cmake_policy(SET CMP0058 NEW)

add_custom_target(gen_all_lua)

MESSAGE(STATUS "Build Core...")
ADD_SUBDIRECTORY(core obj/core/)

IF (CMAKE_BUILD_TYPE MATCHES "Debug")
MESSAGE(STATUS "Build Debug...")
SET(CMAKE_BUILD_TYPE Debug)
message(STATUS "CMAKE_CXX_FLAGS:" ${CMAKE_CXX_FLAGS_DEBUG})
ADD_SUBDIRECTORY(src obj/debug)
ELSE()
MESSAGE(STATUS "Build Release...")
SET(CMAKE_BUILD_TYPE Release)
message(STATUS "CMAKE_CXX_FLAGS:" ${CMAKE_CXX_FLAGS_RELEASE})
ADD_SUBDIRECTORY(src obj/release)
ENDIF()



add_custom_target(format 
  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/format-file.sh core
  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/format-file.sh src
  COMMENT "clang-format all file"
 )


MESSAGE(STATUS ${CMAKE_BUILD_TYPE})
