
include_directories(${ROOT}/network)

add_executable(server_example server_example.cpp)
set_target_properties(server_example PROPERTIES FOLDER "example")
# target_link_libraries(async_client_test gmock)
set_target_workdir(server_example)