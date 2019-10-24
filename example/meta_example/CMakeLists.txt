include(../../cmake/common.cmake)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB SRC "*.h" "*.hpp" "*.cpp")

add_executable(meta_example ${SRC})
set_target_properties(meta_example PROPERTIES FOLDER "example")
target_link_libraries(meta_example ${COMMON_LIBS})
set_target_workdir(meta_example)
target_use_pch(meta_example)