include(../../cmake/common.cmake)
include_directories(${ROOT})

add_executable(async_client_example async_client_example.cpp)
set_target_properties(async_client_example PROPERTIES FOLDER "example")
target_link_libraries(async_client_example ${COMMON_LIBS})
set_target_workdir(async_client_example)
target_use_pch(async_client_example)