
include_directories(${ROOT}/network)

add_executable(bench_client bench_client.cpp)
set_target_properties(bench_client PROPERTIES FOLDER "example")
# target_link_libraries(async_client_test gmock)