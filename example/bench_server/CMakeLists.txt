
include_directories(${ROOT}/network)

add_executable(bench_server bench_server.cpp)
set_target_properties(bench_server PROPERTIES FOLDER "example")
# target_link_libraries(async_client_test gmock)