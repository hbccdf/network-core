include(../../cmake/common.cmake)

if(MSVC)
    set(MYSQL_LIB_NAME libmysql)
else()
    include(FindMySQL)
    set(MYSQL_LIB_NAME mysqlclient)
endif()

add_executable(mysql_example mysql_example.cpp)
set_target_properties(mysql_example PROPERTIES FOLDER "example")
target_link_libraries(mysql_example ${COMMON_LIBS} ${MYSQL_LIB_NAME})
set_target_workdir(mysql_example)
target_use_pch(mysql_example)