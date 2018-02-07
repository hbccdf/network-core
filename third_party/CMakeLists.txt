add_subdirectory(googletest-release-1.8.0)

if(NOT MSVC OR VGDB_LINUX)
   add_subdirectory(thrift-0.10.0_linux/thrift)
endif()
