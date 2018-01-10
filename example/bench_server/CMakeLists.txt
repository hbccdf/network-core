include(../../cmake/common.cmake)
include_directories(${ROOT})

add_executable(bench_server bench_server.cpp)
set_target_properties(bench_server PROPERTIES FOLDER "example")
target_link_libraries(bench_server  ${COMMON_LIBS})
set_target_workdir(bench_server)