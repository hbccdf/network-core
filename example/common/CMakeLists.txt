
include_directories(${ROOT}/network ${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB SRC "*.h" "*.hpp" "*.cpp")
add_files(SRC false "proto")

add_library(server_common ${SRC})
set_target_properties(server_common PROPERTIES FOLDER "server")
set_target_workdir(server_common)
target_use_pch(server_common)