include(../../../cmake/common.cmake)

if(NOT MSVC)
     set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}../)

file(GLOB SRC "*.h" "*.hpp" "*.cpp")
add_files(SRC false "protocol")
add_files(SRC false "transport")

add_library(thrift ${SRC})
set_target_properties(thrift PROPERTIES FOLDER "common")
set_target_workdir(thrift)
#target_use_pch(thrift)