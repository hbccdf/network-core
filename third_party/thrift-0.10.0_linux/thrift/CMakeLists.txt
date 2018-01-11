include(../../../cmake/common.cmake)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}../)

file(GLOB SRC "*.h" "*.hpp" "*.cpp")
add_files(SRC false "protocol")
add_files(SRC false "transport")

add_library(thrift ${SRC})
set_target_properties(thrift PROPERTIES FOLDER "common")
set_target_workdir(thrift)
#target_use_pch(thrift)