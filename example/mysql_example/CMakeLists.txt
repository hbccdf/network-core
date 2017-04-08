
include_directories(${ROOT}/network)

add_executable(mysql_example mysql_example.cpp)
set_target_properties(mysql_example PROPERTIES FOLDER "example")
target_link_libraries(mysql_example ${MYSQL_LIB_NAME})
set_target_workdir(mysql_example)
target_use_pch(mysql_example)