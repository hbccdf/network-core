
include_directories(${ROOT}/network)

add_executable(async_client_example async_client_example.cpp)
set_target_properties(async_client_example PROPERTIES FOLDER "example")
# target_link_libraries(async_client_test gmock)
set_target_workdir(async_client_example)
target_use_pch(async_client_example)