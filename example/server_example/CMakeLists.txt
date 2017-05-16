include(../../cmake/common.cmake)
include_directories(${ROOT})

add_executable(server_example server_example.cpp)
set_target_properties(server_example PROPERTIES FOLDER "example")
target_link_libraries(server_example ${COMMON_LIBS})
set_target_workdir(server_example)