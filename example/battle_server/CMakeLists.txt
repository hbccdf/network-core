include(../../cmake/common.cmake)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${ROOT}/example/common)

file(GLOB SRC "*.h" "*.hpp" "*.cpp")
add_files(SRC false "battle")
add_files(SRC false "new_proto")
add_files(SRC false "service")

add_executable(battle_server ${SRC})
set_target_properties(battle_server PROPERTIES FOLDER "server")
target_link_libraries(battle_server ${COMMON_LIBS} server_common ${THRIFT_LIB_NAME})
set_target_workdir(battle_server)
target_use_pch(battle_server)