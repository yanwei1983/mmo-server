MESSAGE(STATUS "cmake: test-")


ADD_SUBDIRECTORY(db_tester)
ADD_SUBDIRECTORY(zset_tester)
ADD_SUBDIRECTORY(static_reflection_test)
ADD_SUBDIRECTORY(cryptopp_test)
ADD_SUBDIRECTORY(remote_imgui_server_test)
