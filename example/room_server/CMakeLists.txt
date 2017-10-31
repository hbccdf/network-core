
include_directories(${ROOT}/network ${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB SRC "*.h" "*.hpp" "*.cpp")
add_files(SRC false "proto")
add_files(SRC false "new_proto")

add_executable(room_server ${SRC})
set_target_properties(room_server PROPERTIES FOLDER "server")
target_link_libraries(room_server ${THRIFT_LIB_NAME})
set_target_workdir(room_server)
target_use_pch(room_server)