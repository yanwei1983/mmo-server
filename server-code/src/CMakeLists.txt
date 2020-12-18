IF (CMAKE_BUILD_TYPE MATCHES "Debug")
MESSAGE(STATUS "cmake_type: (debug)")
ELSE()
MESSAGE(STATUS "cmake_type: (release)")
ENDIF()

#set(CMAKE_DEPENDS_IN_PROJECT_ONLY ON)
include_directories(${PROJECT_SOURCE_DIR}/src/)
include_directories(${PROJECT_SOURCE_DIR}/src/server_share/)
include_directories(${PROJECT_SOURCE_DIR}/src/game_comm/proto/)
include_directories(${PROJECT_SOURCE_DIR}/src/game_comm/proto/common/)
include_directories(${PROJECT_SOURCE_DIR}/src/game_comm/mydbtable/)
include_directories(${PROJECT_SOURCE_DIR}/src/game_comm/mydbtable/db_proto)
include_directories(${PROJECT_SOURCE_DIR}/src/game_comm/gamemap/)
include_directories(${PROJECT_SOURCE_DIR}/src/game_comm/remote_imgui/imgui_comm/)
include_directories(${PROJECT_SOURCE_DIR}/src/game_comm/remote_imgui/imgui_server/)



ADD_SUBDIRECTORY(game_comm)
ADD_SUBDIRECTORY(service)
ADD_SUBDIRECTORY(tools)
#ADD_SUBDIRECTORY(test)






