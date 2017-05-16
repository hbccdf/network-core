include(../../cmake/common.cmake)
include_directories(${ROOT})

add_executable(bench_client bench_client.cpp)
set_target_properties(bench_client PROPERTIES FOLDER "example")
target_link_libraries(bench_client ${COMMON_LIBS})
set_target_workdir(bench_client)