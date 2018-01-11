cmake_minimum_required(VERSION 2.8)

find_program(CCACHE_FOUND ccache)
if(CCACHE_FOUND)
  message("finded ccahce")
  set(CMAKE_C_COMPILER "ccache")
  set(CMAKE_C_COMPILER_ARG1 "gcc")

  set(CMAKE_CXX_COMPILER "ccache")
  set(CMAKE_CXX_COMPILER_ARG1 "g++")
endif(CCACHE_FOUND)

project(netrowk-core)

set(CMAKE_BUILD_TYPE DEBUG)
set(CMAKE_CONFIGURATION_TYPES "Debug;Release")

# set build output path
#set(CMAKE_SKIP_BUILD_RPATH TRUE)

get_filename_component(ROOT ${CMAKE_CURRENT_LIST_FILE} PATH)
set(CMAKE_MODULE_PATH ${ROOT}/cmake)
set(IN_BATCH 1)
include(vgdb)
include(common)
set(INCLUDED_COMMON 1)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "other")

# build thrid party

# build programs
add_subdirectory(third_party)
add_subdirectory(network)
add_subdirectory(example)
add_subdirectory(test)


#set(CMAKE_VERBOSE_MAKEFILE ON)
