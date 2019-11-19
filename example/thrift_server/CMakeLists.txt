include(../../cmake/common.cmake)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${ROOT}/example/common)

file(GLOB SRC "*.h" "*.hpp" "*.cpp")

add_executable(thrift_server ${SRC})

set_target_properties(thrift_server PROPERTIES FOLDER "example")
target_link_libraries(thrift_server ${COMMON_LIBS} server_common ${THRIFT_LIB_NAME})
set_target_workdir(thrift_server)