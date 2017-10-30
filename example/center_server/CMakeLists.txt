
include_directories(${ROOT}/network)

file(GLOB SRC "*.h" "*.hpp" "*.cpp")
add_files(SRC false "proto")

add_executable(center_server ${SRC})
set_target_properties(center_server PROPERTIES FOLDER "server")
target_link_libraries(center_server ${THRIFT_LIB_NAME})
set_target_workdir(center_server)
target_use_pch(center_server)