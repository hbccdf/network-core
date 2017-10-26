
include_directories(${ROOT}/network)

file(GLOB SRC "*.h" "*.hpp" "*.cpp")
add_files(SRC false "proto")

add_executable(room_server ${SRC})
set_target_properties(room_server PROPERTIES FOLDER "server")
# target_link_libraries(async_client_test gmock)
set_target_workdir(room_server)